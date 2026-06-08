#include "PresetManager.h"
#include "InstrumentProfiles.h"

PresetManager::PresetManager()
{
    loadFactoryPresets();
}

void PresetManager::loadFactoryPresets()
{
    factoryPresets.clear();
    factoryPresets.reserve(OrientalConstants::TOTAL_PRESETS);

    // Factory presets will be loaded from JSON resource
    // For now, placeholder structure matching the 70 presets from prototype
    // Full data will be embedded in factory_presets.json

    auto addPreset = [&](OrientalConstants::Instrument inst,
                         const char* name, const char* city,
                         const char* key, const char* maqam,
                         const char* sound, const char* tags)
    {
        PresetData p;
        p.instrument = inst;
        p.name = name;
        p.city = city;
        p.key = key;
        p.maqam = maqam;
        p.soundDescription = sound;
        p.tags = tags;
        factoryPresets.push_back(p);
    };

    using Inst = OrientalConstants::Instrument;

    // VIOLIN (10)
    addPreset(Inst::Violin, "Mission Cleopatre", "Cairo", "Dm", "Rast", "Legato strings warm, vibrato egyptien", "cinema");
    addPreset(Inst::Violin, "Desert Cinema Score", "Egypt", "Am", "Hijaz", "Tremolo sul ponticello, tension drama", "score");
    addPreset(Inst::Violin, "Casablanca Nights", "Morocco", "Fm", "Saba", "Pizzicato jazz oriental, swing arabe", "jazz");
    addPreset(Inst::Violin, "Atlas Tears", "Algeria", "Bm", "Kurd", "Harmonique flageolet, larme berbere", "emotion");
    addPreset(Inst::Violin, "Beirut Lovers", "Lebanon", "Cm", "Ajam", "Double corde romantique, vibrato lent", "amour");
    addPreset(Inst::Violin, "Oran Sunset Strings", "Algeria", "Em", "Nahawand", "Ensemble cordes, coucher de soleil", "ambient");
    addPreset(Inst::Violin, "Chaabi Wedding Strings", "Morocco", "Am", "Rast", "Staccato festif, ornements chaabi", "festif");
    addPreset(Inst::Violin, "Midnight Desert Scene", "Libya", "Dm", "Hijaz Kar", "Solo lent sur fond pad sombre", "mystere");
    addPreset(Inst::Violin, "Oriental Drama", "Jordan", "Fm", "Saba Zamzam", "Tutti cordes dramatique, fff", "drama");
    addPreset(Inst::Violin, "Aleppo Lament", "Syria", "Gm", "Bayati", "Solo plaintif, portamento douloureux", "nostalgie");

    // OUD (10)
    addPreset(Inst::Oud, "Aladin Night Market", "Morocco", "Bm", "Hijaz", "Oud sec et percussif, attaque claquee", "groove");
    addPreset(Inst::Oud, "Thousand Nights Story", "Iraq", "Dm", "Maqam Rast", "Taqsim improvisation pur, libre", "classique");
    addPreset(Inst::Oud, "Umm Kulthum Tribute", "Egypt", "Gm", "Bayati", "Cordes metalliques brillantes, tarab", "tarab");
    addPreset(Inst::Oud, "Babylon Gate", "Iraq", "Dm", "Siga", "Oud basse profonde, grave mesopotamien", "ancestral");
    addPreset(Inst::Oud, "Desert Caravan", "Saudi", "Dm", "Hijaz", "Tremolo oud sous le vent de sable", "voyage");
    addPreset(Inst::Oud, "Fes Medina", "Morocco", "Bm", "Rahat al-Arwah", "Picking andalou raffine, medina", "andalou");
    addPreset(Inst::Oud, "Palmyra Ghost", "Syria", "Fm", "Nawa Athar", "Harmoniques spectraux, reverbere", "ethere");
    addPreset(Inst::Oud, "Salalah Rain", "Oman", "Dm", "Rast", "Notes perlees sous la pluie d'Oman", "pluie");
    addPreset(Inst::Oud, "Amman Dusk", "Jordan", "Em", "Kurd", "Oud electroacoustique, filtre ouvert", "fusion");
    addPreset(Inst::Oud, "Tangier Port", "Morocco", "Fm", "Jiharkah", "Cordes en soie, port mediterraneen", "mer");

    // NEY (10)
    addPreset(Inst::Ney, "Desert Wind Whisper", "Algeria", "Gm", "Rast", "Souffle ney quasi-flute, pp pianissimo", "meditation");
    addPreset(Inst::Ney, "Luxor Temple Wind", "Egypt", "Dm", "Hijaz", "Ney pharaonique grave, air chaud", "temple");
    addPreset(Inst::Ney, "Istanbul Bridge", "Turkey", "Bm", "Segah", "Ney ottoman classique Derviche", "soufi");
    addPreset(Inst::Ney, "Wadi Rum Sunrise", "Jordan", "Am", "Rast", "Ney solo aube dans le wadi", "aube");
    addPreset(Inst::Ney, "Medina Minarets", "Saudi", "Am", "Saba", "Appel spirituel, maqam saba doux", "sacre");
    addPreset(Inst::Ney, "Atlas Mountain Air", "Morocco", "Bm", "Kurd", "Air de haute altitude, ambiant 4000m", "montagne");
    addPreset(Inst::Ney, "Euphrates Breeze", "Iraq", "Am", "Nawa Athar", "Brise fluviale entre roseaux verts", "fleuve");
    addPreset(Inst::Ney, "Musandam Cliffs", "Oman", "Em", "Nahawand", "Echo sur falaises omaniennes", "falaise");
    addPreset(Inst::Ney, "Carthage Ruins", "Tunisia", "Bm", "Zanjaran", "Vent dans les colonnes antiques", "ruines");
    addPreset(Inst::Ney, "Sinai Whisper", "Egypt", "Gm", "Bayati", "Ney bedouin campement nuit etoilee", "nuit");

    // QANUN (10)
    addPreset(Inst::Qanun, "Alexandria Elegance", "Egypt", "Gm", "Hijaz Kar", "Qanun crystal clair, ornements rapides", "elegance");
    addPreset(Inst::Qanun, "Damascus Ornament", "Syria", "Dm", "Bayati", "Mordants et trilles damascenes raffines", "classique");
    addPreset(Inst::Qanun, "Tunis Malouf", "Tunisia", "Bm", "Rast", "Malouf traditionnel, nouba tunisienne", "malouf");
    addPreset(Inst::Qanun, "Baghdad Maqam", "Iraq", "Em", "Hijaz", "Maqam irakien authentique, profond", "maqam");
    addPreset(Inst::Qanun, "Andalus Revival", "Morocco", "Cm", "Rast", "Musique andalouse mozarabique", "andalou");
    addPreset(Inst::Qanun, "Beirut Jazz Fusion", "Lebanon", "Gm", "Nahawand", "Qanun electrique jazz-oriental fusion", "jazz");
    addPreset(Inst::Qanun, "Tarab Trance", "Egypt", "Fm", "Bayati", "Tarab intense, emotion collective", "tarab");
    addPreset(Inst::Qanun, "Sultanate Strings", "Oman", "Dm", "Ajam", "Qanun royal oman, palais sultanat", "royal");
    addPreset(Inst::Qanun, "Gulf Pearl", "Bahrain", "Am", "Hijaz", "Son perle du golfe, chant des pecheurs", "golfe");
    addPreset(Inst::Qanun, "Byblos Ancient", "Lebanon", "Dm", "Kurd", "Resonances pheniciennes antiques", "antique");

    // DARBUKA (10)
    addPreset(Inst::Darbuka, "Chaabi Rooftop", "Algeria", "Dm", "-", "Doum-tek groovy chaabi toit d'Alger", "chaabi");
    addPreset(Inst::Darbuka, "Casbah Party", "Algeria", "Am", "-", "Rythme festif casbah 140bpm", "fete");
    addPreset(Inst::Darbuka, "Rai Boulevard", "Oran", "Bm", "-", "Groove rai electro-acoustique", "rai");
    addPreset(Inst::Darbuka, "Doumbek Drill", "Lebanon", "Fm", "-", "Tek-ka-doum ultra precis, main droite", "technique");
    addPreset(Inst::Darbuka, "Fes Festival Gnawa", "Morocco", "Am", "-", "Percussions gnawa lilas et jilala", "gnawa");
    addPreset(Inst::Darbuka, "Baghdad Funk", "Iraq", "Am", "-", "Darbuka groove funky maqam irakien", "funk");
    addPreset(Inst::Darbuka, "Nubian Pulse", "Sudan", "Fm", "-", "Pulsation nubienne chaleur intense", "nubian");
    addPreset(Inst::Darbuka, "Riyadh Trap", "Saudi", "Fm", "-", "Hi-hat trap arabe 808 orientalise", "trap");
    addPreset(Inst::Darbuka, "Tripoli Trance", "Libya", "Cm", "-", "Transe hypnotique repetitive 99bpm", "transe");
    addPreset(Inst::Darbuka, "Sousse Riddim", "Tunisia", "Fm", "-", "Riddim tunisien basse saturee", "riddim");

    // RABABAH (10)
    addPreset(Inst::Rababah, "Bedouin Fire", "Jordan", "Gm", "Hijaz", "Rababah bedouine autour du feu", "feu");
    addPreset(Inst::Rababah, "Sinai Nomad", "Egypt", "Em", "Kurd", "Archet long, plaine desertique vaste", "nomade");
    addPreset(Inst::Rababah, "Nejd Plains", "Saudi", "Cm", "Hijaz Kar", "Plainte des plaines du Nejd", "plainte");
    addPreset(Inst::Rababah, "Hadhramaut Valley", "Yemen", "Cm", "Bayati", "Rababah yemenite grave et rauque", "yemen");
    addPreset(Inst::Rababah, "Palmyra Caravan", "Syria", "Dm", "Rast", "Melopee de caravane sous les etoiles", "caravane");
    addPreset(Inst::Rababah, "Wahiba Sands", "Oman", "Am", "Saba", "Sables infinis wahiba, echo lointain", "sable");
    addPreset(Inst::Rababah, "Rub al Khali Soul", "Saudi", "Fm", "Hijaz", "Ame du quart vide, instrument unique", "ame");
    addPreset(Inst::Rababah, "Wadi Hadramawt", "Yemen", "Am", "Nahawand", "Profondeur du wadi, corde resineuse", "profond");
    addPreset(Inst::Rababah, "Tabuk Desert", "Saudi", "Bm", "Rast", "Solitude absolue, note tenue 8 sec", "solitude");
    addPreset(Inst::Rababah, "Sahara Storyteller", "Algeria", "Am", "Bayati", "Griot saharien, histoire orale chantee", "griot");

    // MIZMAR (10)
    addPreset(Inst::Mizmar, "Aswan Festival", "Egypt", "Gm", "Hijaz Kar", "Mizmar procession festival Assouan", "procession");
    addPreset(Inst::Mizmar, "Wedding Procession", "Morocco", "Em", "Rast", "Hautbois double mariage marocain", "mariage");
    addPreset(Inst::Mizmar, "Luxor Procession", "Egypt", "Cm", "Bayati", "Cortege pharaonique sur le Nil", "nil");
    addPreset(Inst::Mizmar, "Jeddah Seafront", "Saudi", "Fm", "Hijaz", "Mizmar port de Jeddah, mers chaudes", "mer");
    addPreset(Inst::Mizmar, "Giza Dawn", "Egypt", "Dm", "Saba", "Aube aux pyramides, premiers rayons", "aube");
    addPreset(Inst::Mizmar, "Khartoum Sufi", "Sudan", "Em", "Kurd", "Zikr soufi khartoum, cercle de priere", "soufi");
    addPreset(Inst::Mizmar, "Jerash Ruins", "Jordan", "Cm", "Nahawand", "Echo romain-arabe ruines de Jerash", "ruines");
    addPreset(Inst::Mizmar, "Sfax Coastal", "Tunisia", "Dm", "Rast", "Brise cotiere, mizmar de pecheur", "cote");
    addPreset(Inst::Mizmar, "Aden Harbor", "Yemen", "Am", "Hijaz", "Port d'Aden, croisement d'oceans", "port");
    addPreset(Inst::Mizmar, "Omdurman Dust", "Sudan", "Bm", "Bayati", "Poussiere d'Omdurman, vent chaud sec", "vent");
}

int PresetManager::getNumPresets() const
{
    return static_cast<int>(factoryPresets.size());
}

int PresetManager::getNumPresetsForInstrument(OrientalConstants::Instrument inst) const
{
    int count = 0;
    for (const auto& p : factoryPresets)
        if (p.instrument == inst) count++;
    return count;
}

const PresetData& PresetManager::getPreset(int index) const
{
    jassert(index >= 0 && index < static_cast<int>(factoryPresets.size()));
    return factoryPresets[static_cast<size_t>(index)];
}

const PresetData& PresetManager::getPreset(OrientalConstants::Instrument inst, int index) const
{
    int count = 0;
    for (const auto& p : factoryPresets)
    {
        if (p.instrument == inst)
        {
            if (count == index) return p;
            count++;
        }
    }
    jassertfalse;
    return factoryPresets[0];
}

void PresetManager::selectPreset(int index)
{
    if (index >= 0 && index < static_cast<int>(factoryPresets.size()))
        currentPresetIndex = index;
}

void PresetManager::selectPreset(OrientalConstants::Instrument inst, int index)
{
    int count = 0;
    for (int i = 0; i < static_cast<int>(factoryPresets.size()); ++i)
    {
        if (factoryPresets[static_cast<size_t>(i)].instrument == inst)
        {
            if (count == index) { currentPresetIndex = i; return; }
            count++;
        }
    }
}

int PresetManager::getCurrentPresetIndex() const
{
    return currentPresetIndex;
}

const PresetData& PresetManager::getCurrentPreset() const
{
    return factoryPresets[static_cast<size_t>(currentPresetIndex)];
}

void PresetManager::saveUserPreset(const PresetData& preset, const juce::File& file)
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("name", preset.name);
    obj->setProperty("city", preset.city);
    obj->setProperty("key", preset.key);
    obj->setProperty("maqam", preset.maqam);
    obj->setProperty("glide", preset.glide);
    obj->setProperty("space", preset.space);
    obj->setProperty("filter", preset.filter);
    obj->setProperty("orient", preset.orient);
    obj->setProperty("reso", preset.reso);
    obj->setProperty("depth", preset.depth);
    obj->setProperty("fxMix", preset.fxMix);

    juce::var json(obj.get());
    file.replaceWithText(juce::JSON::toString(json));
}

PresetData PresetManager::loadUserPreset(const juce::File& file)
{
    PresetData preset;
    auto json = juce::JSON::parse(file);
    if (auto* obj = json.getDynamicObject())
    {
        preset.name   = obj->getProperty("name").toString();
        preset.city   = obj->getProperty("city").toString();
        preset.key    = obj->getProperty("key").toString();
        preset.maqam  = obj->getProperty("maqam").toString();
        preset.glide  = static_cast<float>(obj->getProperty("glide"));
        preset.space  = static_cast<float>(obj->getProperty("space"));
        preset.filter = static_cast<float>(obj->getProperty("filter"));
        preset.orient = static_cast<float>(obj->getProperty("orient"));
        preset.reso   = static_cast<float>(obj->getProperty("reso"));
        preset.depth  = static_cast<float>(obj->getProperty("depth"));
        preset.fxMix  = static_cast<float>(obj->getProperty("fxMix"));
    }
    return preset;
}
