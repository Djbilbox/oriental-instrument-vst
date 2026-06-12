#pragma once
#include <JuceHeader.h>
#include "PresetData.h"
#include "../Utils/Constants.h"
#include <array>
#include <vector>
#include <algorithm>
#include <initializer_list>

// ════════════════════════════════════════════════════════════════════════════
//  Extended preset bank — 210 presets, each with a DISTINCT sonic identity.
//
//  Two presets sound different only if they (a) pull different source samples
//  and/or (b) shape them differently. This generator gives every preset BOTH:
//
//   • sampleFolder — a real folder from the user's sample bank (rotated per
//     instrument tab) so the SOURCE material differs preset to preset.
//   • a tone "character" (10 archetypes) — filter / reso / orient / depth / ADSR
//     / FX combos that the processor's sample-path shaper makes audible.
//
//  7 instrument tabs × 30 presets = 210, appended after the 70 factory presets.
// ════════════════════════════════════════════════════════════════════════════

class PresetDataExtended
{
public:
    // One tone archetype: macros + FX + descriptive flavour.
    struct Character
    {
        float glide, space, filter, orient, reso, depth, fxMix;
        bool  rev, echo, cho, dist, comp, eq, phas, bit;
        float revAmt, echoAmt, choAmt;
        const char* desc;
        const char* tags;
    };

    static const std::array<Character, 10>& characters()
    {
        static const std::array<Character, 10> c = {{
            //gl  sp  fl  or  re  de  fx   rev ech cho dis cmp eq  pha bit  rAmt  eAmt  cAmt  desc                       tags
            {  5, 40, 90, 45, 55, 45, 45,  1,  0,  1,  0,  1,  0,  0,  0,  0.50f,0.40f,0.38f, "bright plucked attack",   "pluck,bright"   },
            { 40, 65, 60, 55, 25, 82, 58,  1,  0,  1,  0,  1,  0,  0,  0,  0.68f,0.35f,0.32f, "warm legato body",        "legato,warm"    },
            { 45, 88, 38, 50, 20, 92, 75,  1,  0,  0,  0,  1,  0,  0,  0,  0.85f,0.18f,0.10f, "dark sustained pad",      "pad,dark"       },
            { 20, 55, 75, 82, 60, 60, 52,  1,  1,  1,  0,  1,  0,  0,  0,  0.55f,0.48f,0.35f, "nasal cutting lead",      "lead,nasal"     },
            {  8, 55, 45, 18, 30, 88, 50,  1,  0,  0,  0,  1,  0,  0,  0,  0.60f,0.30f,0.20f, "deep round body",         "deep,warm"      },
            { 32, 80, 82, 65, 28, 55, 68,  1,  0,  0,  0,  1,  0,  1,  0,  0.80f,0.22f,0.14f, "airy soft presence",      "airy,soft"      },
            { 30, 70, 68, 72, 75, 70, 60,  1,  0,  1,  0,  1,  0,  0,  0,  0.70f,0.38f,0.30f, "resonant vocal cry",      "cry,expressive" },
            {  6, 45, 40, 12, 35, 90, 48,  0,  0,  0,  1,  1,  0,  0,  0,  0.40f,0.35f,0.18f, "sub-heavy foundation",    "bass,deep"      },
            { 12, 48, 86, 38, 48, 62, 46,  0,  1,  1,  1,  0,  0,  0,  0,  0.42f,0.50f,0.42f, "festive bright cut",      "festive,bright" },
            { 35, 85, 62, 55, 40, 80, 70,  1,  0,  0,  0,  1,  0,  1,  0,  0.82f,0.20f,0.16f, "hypnotic trance wash",    "trance,hypnotic"},
        }};
        return c;
    }

    static std::vector<PresetData> loadAll()
    {
        using Inst = OrientalConstants::Instrument;
        std::vector<PresetData> out;
        out.reserve(210);

        // ── Source-folder pools per instrument tab (relative to Samples root).
        //    Rotating these gives each preset different SOURCE material. ──
        const char* P = "sample pour vst ORIENTAL/";

        struct Block
        {
            Inst inst;
            std::vector<juce::String> folders;
            std::vector<const char*>  names;
            std::vector<const char*>  cities;
            std::vector<const char*>  maqams;
        };

        const juce::String ONE = juce::String(P) + "ONE SHOT/";
        const juce::String KIT = juce::String(P) + "! ORIENT KIT/";
        const juce::String WP  = juce::String(P) + "World Percussion Sample Pack/World Percussion Sample Pack/";

        std::vector<Block> blocks;

        // ── VIOLIN / strings ──────────────────────────────────────────────────
        blocks.push_back({ Inst::Violin,
            { "Violin", ONE + "Strings", ONE + "Orchestral/Strings", ONE + "Pads",
              ONE + "Chord", "Rababah", ONE + "Orchestral/Tenor Sax" },
            { "Umm Kulthum Gold","Night at the Citadel","Aleppo Silk Road","Bedouin Stars",
              "Beirut Cafe Jazz","Marrakech Medina","Casablanca Romance","Baghdad Requiem",
              "Isfahan Spring","Fez Trance Ritual","Cairo Cinema Score","Damascus Dawn",
              "Andalus Lament","Oran Sunset","Tripoli Strings","Sana'a Elegy",
              "Petra Echoes","Tunis Malouf","Granada Reflections","Mosul Memory",
              "Sahara Serenade","Nile Nocturne","Atlas Tears","Levant Romance",
              "Phoenician Voyage","Carthage Ruins","Amman Nights","Jerash Procession",
              "Byblos Ancient","Tyre Harbor" },
            { "Cairo","Cairo","Syria","Saudi Arabia","Lebanon","Morocco","Morocco","Iraq",
              "Iran","Morocco","Cairo","Syria","Andalusia","Algeria","Libya","Yemen",
              "Jordan","Tunisia","Spain","Iraq","Sahara","Egypt","Algeria","Levant",
              "Phoenicia","Tunisia","Jordan","Jordan","Lebanon","Lebanon" },
            { "Bayati","Rast","Hijaz","Kurd","Ajam","Rast","Saba","Hijaz",
              "Bayati","Rast","Nahawand","Rast","Saba","Nahawand","Hijaz","Bayati",
              "Kurd","Bayati","Rast","Hijaz","Saba","Nahawand","Kurd","Ajam",
              "Rast","Bayati","Kurd","Rast","Hijaz","Ajam" } });

        // ── OUD / pluck ───────────────────────────────────────────────────────
        blocks.push_back({ Inst::Oud,
            { "Oud", juce::String(P) + "Oud Sample Pack/Oud Sample Pack", ONE + "PLUCK",
              ONE + "Guitar/Guitar/Acoustic Guitar 01", ONE + "Guitar/Guitar/Electric Guitar 01",
              "Qanun" },
            { "Aladin Night Market","Thousand Nights","Umm Kulthum Tarab","Babylon Gate",
              "Desert Caravan","Fes Medina","Palmyra Ghost","Salalah Rain",
              "Amman Dusk","Tangier Port","Baghdad Blues","Beirut Jazz Oud",
              "Damascus Garden","Aladdin Jewel","Sahara Sunset","Cairo Midnight",
              "Istanbul Harbor","Khartoum Festival","Tunis Andalou","Luxor Temple",
              "Mosul Lament","Basra Pearl","Aleppo Souk","Marrakech Riad",
              "Sidon Strings","Nineveh Dawn","Kufa Verses","Hejaz Caravan",
              "Najaf Nights","Qods Whisper" },
            { "Morocco","Iraq","Egypt","Iraq","Saudi Arabia","Morocco","Syria","Oman",
              "Jordan","Morocco","Iraq","Lebanon","Syria","Arabia","Sahara","Cairo",
              "Turkey","Sudan","Tunisia","Egypt","Iraq","Iraq","Syria","Morocco",
              "Lebanon","Iraq","Iraq","Hejaz","Iraq","Jerusalem" },
            { "Hijaz","Rast","Bayati","Siga","Hijaz","Rast","Nawa Athar","Rast",
              "Kurd","Jiharkah","Hijaz","Ajam","Rast","Bayati","Saba","Hijaz",
              "Rast","Nahawand","Bayati","Rast","Saba","Rast","Bayati","Hijaz",
              "Ajam","Rast","Bayati","Hijaz","Saba","Kurd" } });

        // ── NEY / wind ────────────────────────────────────────────────────────
        blocks.push_back({ Inst::Ney,
            { "Ney", "Mizmar", ONE + "BRASS", ONE + "Orchestral/Trombone", ONE + "LEAD" },
            { "Sufi Meditation","Desert Whisper","Felucca Breeze","Mountain Echo",
              "City Oasis","Bazaar Calls","Mystical Veil","Garden of Eden",
              "Aladdin Quest","Pilgrimage Road","Anatolia Wind","Konya Dervish",
              "Nile Reed","Cappadocia Dawn","Sahara Breath","Bosphorus Mist",
              "Damascus Minaret","Mecca Dusk","Karbala Lament","Yemen Highlands",
              "Hidden Oasis","Whirling Trance","Twilight Caravan","Sinai Wind",
              "Euphrates Calm","Tigris Flow","Star of Sheba","Lost Caravanserai",
              "Moonlit Dunes","Sacred Grove" },
            { "Turkey","Algeria","Egypt","Lebanon","Cairo","Damascus","Istanbul","Eden",
              "Arabia","Holy Land","Anatolia","Konya","Egypt","Cappadocia","Sahara","Istanbul",
              "Syria","Mecca","Iraq","Yemen","Oasis","Konya","Sahara","Sinai",
              "Iraq","Iraq","Sheba","Silk Road","Sahara","Eden" },
            { "Rast","Hijaz","Rast","Bayati","Nahawand","Ajam","Kurd","Rast",
              "Hijaz","Rast","Bayati","Kurd","Rast","Hijaz","Saba","Bayati",
              "Kurd","Rast","Saba","Bayati","Rast","Kurd","Hijaz","Rast",
              "Nahawand","Rast","Bayati","Hijaz","Saba","Rast" } });

        // ── QANUN / keys ──────────────────────────────────────────────────────
        blocks.push_back({ Inst::Qanun,
            { "Qanun", juce::String(P) + "Qanun One Shots/Qanun One Shots",
              ONE + "Piano/Piano 1", ONE + "Piano/Piano 2", ONE + "Bells",
              WP + "Mbira (Thumb Piano, Zimbabwe)", ONE + "PLUCK" },
            { "Sultan Harem","Celestial Hammers","Palace Gardens","Arabian Dream",
              "Silk Caravan","Damascus Courtyard","Sufi Circle","Moroccan Stars",
              "Alhambra Lights","Yemen Treasures","Cairo Conservatory","Baghdad Maqam",
              "Beirut Fusion","Byblos Strings","Gulf Pearl","Tunis Malouf",
              "Andalus Revival","Alexandria Grace","Tarab Trance","Oman Sultanate",
              "Bahrain Breeze","Qatar Mirage","Kuwait Nights","Muscat Dawn",
              "Aden Harbor","Doha Skyline","Sharjah Souk","Riyadh Palace",
              "Jeddah Sea","Medina Calm" },
            { "Istanbul","Egypt","Persia","Egypt","Silk Road","Syria","Turkey","Morocco",
              "Granada","Yemen","Cairo","Iraq","Lebanon","Lebanon","Bahrain","Tunisia",
              "Morocco","Egypt","Egypt","Oman","Bahrain","Qatar","Kuwait","Oman",
              "Yemen","Qatar","UAE","Saudi Arabia","Saudi Arabia","Saudi Arabia" },
            { "Rast","Nahawand","Bayati","Hijaz","Ajam","Saba","Kurd","Rast",
              "Rast","Bayati","Hijaz","Rast","Ajam","Nahawand","Rast","Bayati",
              "Rast","Saba","Hijaz","Rast","Bayati","Hijaz","Kurd","Rast",
              "Bayati","Ajam","Rast","Nahawand","Hijaz","Rast" } });

        // ── DARBUKA / percussion ──────────────────────────────────────────────
        blocks.push_back({ Inst::Darbuka,
            { "Darbuka", KIT + "IVS ARABIC DRUM KIT VOL 2", KIT + "IVS ARABIC DRUM KIT V1",
              KIT + "EGYPTIAN DRUM KIT/KIT 1", KIT + "EGYPTIAN DRUM KIT/KIT 2",
              WP + "Tar (Frame drum, Middle East)", WP + "Riq (Tambourine, Egypt)" },
            { "Baghdad Funk","Casbah Party","Belly Dance","Wedding Drums",
              "Doum Tarab","Takka Roll","Sufi Ritual","Bedouin Fire",
              "Chaabi Rooftop","Aladdin Market","Nubian Pulse","Rai Boulevard",
              "Fes Gnawa","Doumbek Drill","Suq Energy","Saidi Groove",
              "Maqsoum Drive","Malfuf Run","Ayoub Trance","Zar Ceremony",
              "Khaleeji Beat","Dabke Stomp","Masmoudi Heavy","Wahda Slow",
              "Cairo Street","Oran Carnival","Tunis Carnival","Beirut Underground",
              "Amman Drill","Jeddah Trap" },
            { "Iraq","Algeria","Egypt","Morocco","Lebanon","Tunisia","Turkey","Saudi Arabia",
              "Algeria","Arabia","Sudan","Algeria","Morocco","Lebanon","Iraq","Egypt",
              "Egypt","Lebanon","Egypt","Egypt","Gulf","Levant","Tunisia","Egypt",
              "Cairo","Algeria","Tunisia","Lebanon","Jordan","Saudi Arabia" },
            { "Rast","Hijaz","Bayati","Rast","Ajam","Saba","Kurd","Nahawand",
              "Rast","Hijaz","Bayati","Rast","Gnawa","Ajam","Hijaz","Saidi",
              "Rast","Bayati","Hijaz","Saba","Khaleeji","Rast","Bayati","Rast",
              "Hijaz","Rast","Bayati","Kurd","Rast","Hijaz" } });

        // ── RABABAH / bowed ───────────────────────────────────────────────────
        blocks.push_back({ Inst::Rababah,
            { "Rababah", "Violin", ONE + "Strings", ONE + "Orchestral/Strings",
              ONE + "Orchestral/Tenor Sax", ONE + "Pads" },
            { "Bedouin Lament","Desert Night Song","Tuareg Wanderer","Palmyra Ghost",
              "Nomad Prayer","Sudan Soul","Timbuktu Memory","Nejd Plains",
              "Rub al Khali","Sinai Nomad","Hadhramaut Valley","Wadi Rum Sunrise",
              "Tabuk Desert","Wahiba Sands","Sahara Storyteller","Empty Quarter",
              "Bedouin Fire","Caravan Dawn","Oasis Mirage","Dune Sea",
              "Nabatean Trail","Frankincense Road","Salt Flats","Star Navigator",
              "Camel Trek","Majlis Tales","Falcon Flight","Date Palm Grove",
              "Black Tent","Wind of Najd" },
            { "Saudi Arabia","Algeria","Mali","Syria","Arabia","Sudan","Mali","Saudi Arabia",
              "Saudi Arabia","Egypt","Yemen","Jordan","Saudi Arabia","Oman","Algeria","Arabia",
              "Saudi Arabia","Sahara","Sahara","Arabia","Jordan","Oman","Arabia","Sahara",
              "Arabia","Gulf","Arabia","Arabia","Arabia","Najd" },
            { "Kurd","Hijaz","Rast","Bayati","Nahawand","Ajam","Kurd","Rast",
              "Saba","Hijaz","Bayati","Rast","Kurd","Rast","Bayati","Saba",
              "Nahawand","Rast","Hijaz","Kurd","Rast","Bayati","Saba","Rast",
              "Hijaz","Ajam","Rast","Bayati","Kurd","Rast" } });

        // ── MIZMAR / reed-brass ───────────────────────────────────────────────
        blocks.push_back({ Inst::Mizmar,
            { "Mizmar", ONE + "BRASS", ONE + "Orchestral/Trombone",
              ONE + "Orchestral/Tenor Sax", ONE + "LEAD" },
            { "Wedding Procession","Festival Fanfare","Suq Energy","Procession Brass",
              "Zarb Driver","Bedouin Call","Aladdin Joyride","Sultan Entrance",
              "Zikr Chant","Cairo Bazaar","Saidi Parade","Gnawa Horns",
              "Nubian March","Carnival Cry","Harvest Dance","Victory Herald",
              "Mawlid Night","Eid Morning","Henna Night","Tahtib Duel",
              "Upper Nile","Aswan Festival","Luxor Procession","Giza Dawn",
              "Khartoum Sufi","Omdurman Dust","Sfax Coastal","Jerash Ruins",
              "Aden Festival","Hadramawt Call" },
            { "Morocco","Egypt","Baghdad","Tunisia","Iran","Saudi Arabia","Arabia","Ottoman",
              "Islamic","Cairo","Egypt","Morocco","Sudan","Tunisia","Levant","Arabia",
              "Egypt","Egypt","Morocco","Egypt","Sudan","Egypt","Egypt","Egypt",
              "Sudan","Sudan","Tunisia","Jordan","Yemen","Yemen" },
            { "Rast","Bayati","Hijaz","Saba","Ajam","Kurd","Rast","Bayati",
              "Rast","Hijaz","Saidi","Gnawa","Bayati","Rast","Ajam","Rast",
              "Bayati","Rast","Hijaz","Saba","Rast","Bayati","Hijaz","Rast",
              "Kurd","Bayati","Rast","Hijaz","Ajam","Bayati" } });

        const char* keys[] = { "Dm","Am","Gm","Em","Cm","Fm","Bm" };
        const auto& chars = characters();

        for (const auto& blk : blocks)
        {
            // Use the SHORTEST array so a miscount never reads out of bounds.
            const int n = static_cast<int>(std::min({ blk.names.size(),
                                                      blk.cities.size(),
                                                      blk.maqams.size() }));
            for (int i = 0; i < n; ++i)
            {
                const Character& c = chars[static_cast<size_t>(i) % chars.size()];
                const juce::String& folder = blk.folders[static_cast<size_t>(i) % blk.folders.size()];

                PresetData p;
                p.instrument   = blk.inst;
                p.name         = blk.names[static_cast<size_t>(i)];
                p.city         = blk.cities[static_cast<size_t>(i)];
                p.key          = keys[i % 7];
                p.maqam        = blk.maqams[static_cast<size_t>(i)];
                p.soundDescription = c.desc;
                p.tags         = c.tags;
                p.sampleFolder = folder;

                p.glide  = c.glide;  p.space = c.space;  p.filter = c.filter;
                p.orient = c.orient; p.reso  = c.reso;   p.depth  = c.depth;  p.fxMix = c.fxMix;

                p.reverbOn = c.rev;  p.echoOn = c.echo;  p.chorusOn = c.cho;
                p.distortOn = c.dist; p.compressorOn = c.comp; p.eqOn = c.eq;
                p.phaserOn = c.phas; p.bitcrushOn = c.bit;
                p.reverbAmount = c.revAmt; p.echoAmount = c.echoAmt; p.chorusAmount = c.choAmt;

                out.push_back(p);
            }
        }

        return out;
    }
};
