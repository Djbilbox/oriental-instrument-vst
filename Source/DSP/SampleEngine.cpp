#include "SampleEngine.h"
#include "BendableSampler.h"
#include <regex>
#include <vector>
#include <algorithm>
#include <cctype>

namespace
{
    constexpr int kSamplerVoices = 32;
    constexpr double kMaxSampleSeconds = 30.0;
}

SampleEngine::SampleEngine()
{
    formatManager.registerBasicFormats(); // wav, aiff (+ flac/ogg if enabled)
    for (int i = 0; i < kSamplerVoices; ++i)
        synth.addVoice(new BendableVoice()); // responds to pitch + mod wheel
}

void SampleEngine::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize  = samplesPerBlock;
    synth.setCurrentPlaybackSampleRate(sr);
}

juce::File SampleEngine::samplesRoot()
{
    auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                   .getChildFile("Oriental Instrument")
                   .getChildFile("Samples");
    if (! dir.exists())
        dir.createDirectory();
    return dir;
}

// Parse a MIDI root note from a filename. Returns -1 if none found.
// Scientific pitch: C4 = 60. Takes the LAST note-name token in the name, so
// messy library names ("STR_B3RockRot_D#1", "MIXO_A_3(L)ogg") resolve to the
// real root at the end, not a stray letter+digit earlier in the string.
int SampleEngine::parseRootNote(const juce::String& name)
{
    const std::string s = name.toStdString();

    // 1) Note name: letter, optional accidental, optional separator, octave.
    {
        static const std::regex re(R"(([A-Ga-g])([#bB]?)[ _\-]?(-?\d))");
        int best = -1;
        for (auto it = std::sregex_iterator(s.begin(), s.end(), re);
             it != std::sregex_iterator(); ++it)
        {
            const auto& m = *it;
            const char letter = static_cast<char>(std::toupper(m[1].str()[0]));
            const std::string acc = m[2].str();
            const int octave = std::stoi(m[3].str());

            static const int semis[7] = { 9, 11, 0, 2, 4, 5, 7 }; // A B C D E F G
            int pc = semis[letter - 'A'];
            if (acc == "#") pc += 1;
            else if (acc == "b" || acc == "B") pc -= 1;

            const int midi = (octave + 1) * 12 + pc; // C-1 = 0 → C4 = 60
            if (midi >= 0 && midi <= 127)
                best = midi; // keep last valid
        }
        if (best >= 0)
            return best;
    }

    // 2) A bare midi number token, e.g. "_57" or "-60".
    {
        static const std::regex re(R"((^|[ _\-])(\d{1,3})($|[ _\-.]))");
        std::smatch m;
        if (std::regex_search(s, m, re))
        {
            const int n = std::stoi(m[2].str());
            if (n >= 0 && n <= 127)
                return n;
        }
    }

    return -1;
}

void SampleEngine::loadFolder(const juce::File& dir)
{
    synth.clearSounds();
    currentHasSamples = false;

    if (! dir.isDirectory())
        return;

    auto files = dir.findChildFiles(juce::File::findFiles, false, "*.wav;*.aiff;*.aif;*.flac;*.ogg");
    if (files.isEmpty())
        return;

    // Collect (rootNote, file). Files without a parsed root spread evenly later.
    struct Entry { int root; juce::File file; };
    std::vector<Entry> entries;
    for (auto& f : files)
    {
        int root = parseRootNote(f.getFileNameWithoutExtension());
        entries.push_back({ root, f });
    }

    // If nothing parsed, lay files out chromatically from C2 so each still plays.
    bool anyParsed = std::any_of(entries.begin(), entries.end(),
                                 [](const Entry& e) { return e.root >= 0; });
    if (! anyParsed)
        for (size_t i = 0; i < entries.size(); ++i)
            entries[i].root = juce::jlimit(0, 127, 36 + static_cast<int>(i) * 3);

    // Drop any that still have no root, then sort by root.
    entries.erase(std::remove_if(entries.begin(), entries.end(),
                                 [](const Entry& e) { return e.root < 0; }),
                  entries.end());
    if (entries.empty())
        return;
    std::sort(entries.begin(), entries.end(),
              [](const Entry& a, const Entry& b) { return a.root < b.root; });

    // Build key zones: each sample covers the midpoints to its neighbours.
    for (size_t i = 0; i < entries.size(); ++i)
    {
        const int root = entries[i].root;
        const int lo = (i == 0) ? 0
                                : (entries[i - 1].root + root) / 2 + 1;
        const int hi = (i + 1 == entries.size()) ? 127
                                                 : (root + entries[i + 1].root) / 2;

        juce::BigInteger keys;
        keys.setRange(lo, juce::jmax(1, hi - lo + 1), true);

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(entries[i].file));
        if (reader == nullptr)
            continue;

        synth.addSound(new BendableSound(entries[i].file.getFileName(), *reader,
                                         keys, root,
                                         0.002,   // attack (s)
                                         0.15,    // release (s)
                                         kMaxSampleSeconds));
    }

    currentHasSamples = synth.getNumSounds() > 0;
}

void SampleEngine::setInstrument(OrientalConstants::Instrument inst)
{
    if (static_cast<int>(inst) == loadedInstrument && cachedFolderPath.isEmpty())
        return;
    loadedInstrument = static_cast<int>(inst);
    cachedFolderPath = {}; // a path load is now stale; instrument folder is active

    const juce::File dir = samplesRoot().getChildFile(OrientalConstants::instrumentName(inst));
    if (! dir.exists())
        dir.createDirectory(); // so the user sees where to drop files
    loadFolder(dir);
}

void SampleEngine::loadFromPath(const juce::String& relativePath)
{
    if (relativePath.isEmpty())
        return;
    if (relativePath == cachedFolderPath)
        return; // already loaded — LRU no-op
    cachedFolderPath = relativePath;
    loadedInstrument = -1; // instrument cache is now stale

    const juce::File dir = samplesRoot().getChildFile(relativePath);
    loadFolder(dir); // sets currentHasSamples; if empty the processor uses synth
}

void SampleEngine::render(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi,
                          int startSample, int numSamples)
{
    synth.renderNextBlock(buffer, midi, startSample, numSamples);
}

void SampleEngine::allNotesOff()
{
    synth.allNotesOff(0, true);
}
