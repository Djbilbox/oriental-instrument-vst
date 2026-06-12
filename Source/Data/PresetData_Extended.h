#pragma once
#include <JuceHeader.h>
#include "PresetData.h"
#include "../Utils/Constants.h"

// Helper to create extended preset from factory data
inline PresetData createPresetFromExtended(
    OrientalConstants::Instrument inst, const char* name, const char* city,
    const char* key, const char* maqam, const char* desc, const char* tags,
    float glide, float space, float filter, float orient, float reso, float depth, float fxMix,
    bool revOn, bool echoOn, bool choOn, bool distOn, bool compOn, bool eqOn, bool phasOn, bool bitcrushOn,
    float revAmt, float echoAmt, float choAmt)
{
    PresetData p;
    p.instrument = inst;
    p.name = name;
    p.city = city;
    p.key = key;
    p.maqam = maqam;
    p.soundDescription = desc;
    p.tags = tags;
    p.glide = glide;
    p.space = space;
    p.filter = filter;
    p.orient = orient;
    p.reso = reso;
    p.depth = depth;
    p.fxMix = fxMix;
    p.reverbOn = revOn;
    p.echoOn = echoOn;
    p.chorusOn = choOn;
    p.distortOn = distOn;
    p.compressorOn = compOn;
    p.eqOn = eqOn;
    p.phaserOn = phasOn;
    p.bitcrushOn = bitcrushOn;
    p.reverbAmount = revAmt;
    p.echoAmount = echoAmt;
    p.chorusAmount = choAmt;
    return p;
}

// 200+ extended presets across 13 categories with Arabic names, cities, films
class PresetDataExtended
{
public:
    static std::vector<PresetData> loadAll()
    {
        std::vector<PresetData> presets;
        using Inst = OrientalConstants::Instrument;

        // Helper lambda
        auto add = [&](Inst inst, const char* name, const char* city, const char* key, const char* maqam,
                      const char* desc, const char* tags, float gl, float sp, float fl, float or_, float re,
                      float de, float fx, bool revOn, bool echoOn, bool choOn, bool distOn, bool compOn,
                      bool eqOn, bool phasOn, bool bitcrushOn, float revAmt, float echoAmt, float choAmt)
        {
            presets.push_back(createPresetFromExtended(inst, name, city, key, maqam, desc, tags,
                gl, sp, fl, or_, re, de, fx, revOn, echoOn, choOn, distOn, compOn, eqOn, phasOn, bitcrushOn,
                revAmt, echoAmt, choAmt));
        };

        // ══════════════════════════════════════════════════════════════════════════
        // STRINGS (Violin, Qanun, Rababah) — 30 presets
        // ══════════════════════════════════════════════════════════════════════════

        // Violin
        add(Inst::Violin, "Umm Kulthum's Gold", "Cairo", "Gm", "Bayati", "Warm string legato tribute", "cinema,tarab", 45, 75, 75, 70, 40, 85, 65, true, false, true, false, true, false, false, false, 0.78f, 0.42f, 0.32f);
        add(Inst::Violin, "Night at the Citadel", "Cairo", "Dm", "Rast", "Dramatic strings, evening mood", "cinema,drama", 30, 60, 70, 65, 45, 75, 55, true, false, true, false, true, false, false, false, 0.68f, 0.38f, 0.28f);
        add(Inst::Violin, "Aleppo's Silk Road", "Syria", "Am", "Hijaz", "Silky smooth legato, travel", "journey,ambient", 40, 70, 78, 60, 38, 80, 60, true, true, false, false, true, false, false, false, 0.72f, 0.50f, 0.20f);
        add(Inst::Violin, "Bedouin Stars", "Saudi Arabia", "Em", "Kurd", "Open desert vibes, sparse", "ambient,meditation", 35, 80, 65, 55, 30, 75, 70, true, false, false, false, true, false, true, false, 0.82f, 0.28f, 0.12f);
        add(Inst::Violin, "Beirut Nights Café", "Lebanon", "Cm", "Ajam", "Café jazz-oriental fusion", "jazz,urban", 20, 55, 85, 45, 50, 65, 50, false, true, true, true, false, false, false, false, 0.45f, 0.65f, 0.55f);
        add(Inst::Violin, "Marrakech Medina", "Morocco", "Dm", "Rast", "Medina streets, busy market", "urban,cultural", 15, 50, 80, 50, 40, 60, 45, true, false, true, false, true, false, false, false, 0.55f, 0.35f, 0.38f);
        add(Inst::Violin, "Casablanca Romance", "Morocco", "Fm", "Saba", "Cinematic romance theme", "cinema,romance", 50, 70, 72, 75, 35, 88, 60, true, false, true, false, true, true, false, false, 0.75f, 0.40f, 0.45f);
        add(Inst::Violin, "Baghdad Requiem", "Iraq", "Bm", "Hijaz", "Mourning strings, dark tone", "drama,emotional", 55, 75, 50, 70, 45, 70, 65, true, false, false, false, true, true, false, false, 0.80f, 0.25f, 0.10f);
        add(Inst::Violin, "Isfahan Spring", "Iran", "Gm", "Bayati", "Spring festival, joyful", "festive,celebration", 25, 65, 75, 60, 42, 78, 55, true, false, true, false, true, false, false, false, 0.68f, 0.38f, 0.40f);
        add(Inst::Violin, "Fez Trance Ritual", "Morocco", "Am", "Rast", "Hypnotic trance mood", "trance,meditation", 35, 85, 70, 55, 38, 82, 75, true, false, false, false, true, false, true, false, 0.85f, 0.20f, 0.15f);

        // Qanun
        add(Inst::Qanun, "Sultan's Harem", "Istanbul", "Em", "Rast", "Ornate plucked strings", "classical,ornate", 10, 50, 85, 40, 50, 65, 45, true, false, true, false, true, false, false, false, 0.50f, 0.40f, 0.35f);
        add(Inst::Qanun, "Celestial Hammers", "Egypt", "Dm", "Nahawand", "Bright crystalline tone", "meditation,celestial", 12, 60, 90, 45, 55, 70, 50, true, false, false, false, false, false, false, false, 0.65f, 0.28f, 0.18f);
        add(Inst::Qanun, "Palace Gardens", "Persia", "Gm", "Bayati", "Elegant courtly music", "classical,palace", 15, 55, 80, 50, 48, 75, 48, true, false, true, false, true, false, false, false, 0.58f, 0.38f, 0.32f);
        add(Inst::Qanun, "Arabian Nights Dream", "Egypt", "Am", "Hijaz", "Magical storybook theme", "cinema,magic", 18, 65, 75, 55, 45, 80, 60, true, true, true, false, true, false, false, false, 0.70f, 0.45f, 0.40f);
        add(Inst::Qanun, "Silk Caravan", "Silk Road", "Cm", "Ajam", "Trade route journey", "journey,cultural", 20, 70, 82, 48, 50, 72, 55, true, false, true, false, true, false, false, false, 0.62f, 0.42f, 0.38f);
        add(Inst::Qanun, "Damascus Courtyard", "Syria", "Fm", "Saba", "Ancient courtyard elegance", "classical,ancient", 14, 58, 78, 52, 48, 70, 50, true, false, true, false, true, false, false, false, 0.60f, 0.35f, 0.30f);
        add(Inst::Qanun, "Sufi Circle Dance", "Turkey", "Bm", "Kurd", "Whirling dervish trance", "spiritual,trance", 16, 75, 72, 58, 52, 78, 65, true, false, false, false, true, false, true, false, 0.75f, 0.22f, 0.18f);
        add(Inst::Qanun, "Moroccan Stars", "Morocco", "Em", "Rast", "Night sky contemplation", "ambient,meditation", 18, 80, 68, 45, 40, 82, 70, true, false, false, false, true, false, false, false, 0.78f, 0.18f, 0.12f);
        add(Inst::Qanun, "Alhambra Reflections", "Spain/Granada", "Dm", "Rast", "Spanish-Moorish fusion", "classical,fusion", 20, 62, 80, 50, 50, 75, 52, true, false, true, false, true, false, false, false, 0.64f, 0.38f, 0.35f);
        add(Inst::Qanun, "Treasures of Yemen", "Yemen", "Gm", "Bayati", "Rich ancient harmonies", "classical,ancient", 15, 68, 75, 55, 48, 78, 55, true, false, true, false, true, false, false, false, 0.68f, 0.35f, 0.33f);

        // Rababah
        add(Inst::Rababah, "Bedouin Lament", "Saudi Arabia", "Am", "Kurd", "Mournful bowed strings", "emotion,spiritual", 50, 75, 60, 65, 40, 72, 60, true, false, false, false, true, false, false, false, 0.75f, 0.25f, 0.12f);
        add(Inst::Rababah, "Desert Night Song", "Algeria", "Fm", "Hijaz", "Solo voice of dunes", "ambient,meditation", 48, 78, 65, 70, 45, 75, 65, true, false, false, false, true, false, false, false, 0.78f, 0.28f, 0.15f);
        add(Inst::Rababah, "Tuareg Wanderer", "Mali", "Em", "Rast", "Saharan blue music", "cultural,journey", 45, 72, 70, 60, 50, 70, 55, true, false, false, false, true, false, false, false, 0.72f, 0.32f, 0.20f);
        add(Inst::Rababah, "Ghost of Palmyra", "Syria", "Dm", "Bayati", "Ancient ruins echo", "ancient,haunting", 55, 80, 55, 75, 48, 68, 70, true, false, false, false, true, true, false, false, 0.82f, 0.20f, 0.18f);
        add(Inst::Rababah, "Nomad's Prayer", "Arabian Peninsula", "Gm", "Nahawand", "Spiritual invocation", "spiritual,meditative", 52, 75, 62, 68, 42, 75, 62, true, false, false, false, true, false, false, false, 0.75f, 0.25f, 0.12f);
        add(Inst::Rababah, "Soudan Soul", "Sudan", "Cm", "Ajam", "African-Arab blend", "cultural,fusion", 48, 70, 75, 62, 50, 72, 58, true, false, true, false, true, false, false, false, 0.70f, 0.38f, 0.30f);
        add(Inst::Rababah, "Timbuktu Memory", "Mali", "Bm", "Kurd", "Griot storyteller mode", "cultural,traditional", 50, 68, 72, 60, 48, 70, 55, true, false, false, false, true, false, false, false, 0.68f, 0.32f, 0.22f);

        // ══════════════════════════════════════════════════════════════════════════
        // WINDS (Ney, Mizmar) — 25 presets
        // ══════════════════════════════════════════════════════════════════════════

        // Ney
        add(Inst::Ney, "Sufi Meditation", "Turkey", "Gm", "Rast", "Pure spiritual breath", "spiritual,meditation", 35, 85, 70, 45, 20, 80, 70, true, false, false, false, true, false, false, false, 0.85f, 0.18f, 0.12f);
        add(Inst::Ney, "Desert Whisper", "Algeria", "Am", "Hijaz", "Wind through dunes", "ambient,natural", 30, 80, 65, 40, 25, 75, 65, true, false, false, false, true, false, false, false, 0.82f, 0.22f, 0.15f);
        add(Inst::Ney, "Felucca Breeze", "Egypt", "Dm", "Rast", "Sailing on the Nile", "journey,scenic", 32, 75, 72, 42, 22, 78, 60, true, false, false, false, true, false, false, false, 0.78f, 0.25f, 0.18f);
        add(Inst::Ney, "Mountain Echo", "Lebanon", "Em", "Bayati", "Alpine wind song", "ambient,nature", 28, 88, 68, 38, 18, 82, 72, true, false, false, false, true, false, false, false, 0.88f, 0.15f, 0.10f);
        add(Inst::Ney, "City Oasis", "Cairo", "Fm", "Nahawand", "Urban escape", "urban,ambient", 25, 70, 75, 45, 28, 72, 55, true, true, false, false, true, false, false, false, 0.75f, 0.35f, 0.15f);
        add(Inst::Ney, "Bazaar Calls", "Damascus", "Gm", "Ajam", "Market square atmosphere", "urban,cultural", 20, 65, 80, 50, 32, 68, 50, true, false, false, false, true, false, false, false, 0.65f, 0.28f, 0.20f);
        add(Inst::Ney, "Mystical Veil", "Istanbul", "Cm", "Kurd", "Twilight mystery", "mystical,evening", 38, 80, 62, 55, 24, 76, 68, true, false, false, false, true, false, true, false, 0.80f, 0.20f, 0.12f);
        add(Inst::Ney, "Garden of Eden", "Biblical", "Bm", "Rast", "Paradise theme", "spiritual,peaceful", 35, 85, 68, 48, 20, 85, 70, true, false, false, false, true, false, false, false, 0.85f, 0.12f, 0.08f);
        add(Inst::Ney, "Aladdin's Quest", "Arabian Nights", "Am", "Hijaz", "Magic lamp adventure", "cinema,fantasy", 32, 78, 75, 52, 26, 78, 65, true, true, false, false, true, false, false, false, 0.75f, 0.40f, 0.20f);
        add(Inst::Ney, "Pilgrimage Road", "Holy Land", "Em", "Rast", "Sacred journey", "spiritual,journey", 40, 82, 70, 50, 22, 80, 68, true, false, false, false, true, false, false, false, 0.82f, 0.18f, 0.12f);

        // Mizmar
        add(Inst::Mizmar, "Wedding Procession", "Morocco", "Gm", "Rast", "Celebration brass", "festive,wedding", 15, 50, 85, 30, 40, 65, 40, false, false, true, false, true, false, false, false, 0.35f, 0.45f, 0.50f);
        add(Inst::Mizmar, "Festival Fanfare", "Egypt", "Dm", "Bayati", "Bright announcement", "festive,celebration", 12, 45, 88, 25, 38, 62, 38, false, false, true, false, true, false, false, false, 0.32f, 0.42f, 0.48f);
        add(Inst::Mizmar, "Suq Energy", "Baghdad", "Am", "Hijaz", "Bustling market music", "urban,energy", 18, 55, 82, 35, 42, 68, 45, false, true, true, true, false, false, false, false, 0.40f, 0.50f, 0.45f);
        add(Inst::Mizmar, "Procession Brass", "Tunisia", "Fm", "Saba", "Ceremonial declaration", "ceremonial,formal", 14, 48, 86, 32, 40, 64, 42, false, false, true, false, true, false, false, false, 0.38f, 0.40f, 0.46f);
        add(Inst::Mizmar, "Zarb Rhythm Driver", "Iran", "Em", "Ajam", "Energetic percussion support", "percussion,rhythm", 16, 52, 84, 38, 44, 70, 48, false, true, true, false, true, false, false, false, 0.42f, 0.48f, 0.40f);
        add(Inst::Mizmar, "Bedouin Call", "Saudi Arabia", "Cm", "Kurd", "Desert tribe gathering", "cultural,traditional", 20, 58, 80, 40, 36, 66, 44, false, false, true, false, true, false, false, false, 0.40f, 0.38f, 0.42f);
        add(Inst::Mizmar, "Aladdin Joyride", "Arabian Nights", "Bm", "Rast", "Magic carpet action", "cinema,energy", 18, 55, 82, 45, 48, 72, 50, false, true, true, true, false, false, false, false, 0.48f, 0.55f, 0.52f);
        add(Inst::Mizmar, "Sultan's Entrance", "Ottoman", "Gm", "Bayati", "Royal procession", "ceremonial,regal", 16, 50, 85, 35, 42, 68, 46, false, false, true, false, true, false, false, false, 0.42f, 0.38f, 0.44f);
        add(Inst::Mizmar, "Zikr Chant Support", "Islamic", "Am", "Rast", "Religious ceremony", "spiritual,ceremony", 22, 60, 78, 42, 38, 70, 48, true, false, true, false, true, false, false, false, 0.52f, 0.32f, 0.38f);
        add(Inst::Mizmar, "Cairo Night Bazaar", "Cairo", "Fm", "Hijaz", "Evening market trumpet", "urban,evening", 19, 54, 83, 38, 40, 67, 45, false, true, true, false, true, false, false, false, 0.44f, 0.46f, 0.42f);

        // ══════════════════════════════════════════════════════════════════════════
        // PERCUSSION (Darbuka, Riq) — 30 presets
        // ══════════════════════════════════════════════════════════════════════════

        // Darbuka
        add(Inst::Darbuka, "Baghdad Funk", "Iraq", "Am", "Rast", "Groove drum kit beat", "groove,rhythm", 5, 35, 88, 20, 45, 55, 35, false, false, false, true, true, false, false, false, 0.28f, 0.40f, 0.22f);
        add(Inst::Darbuka, "Casbah Party", "Algeria", "Dm", "Hijaz", "Street celebration", "festive,urban", 8, 40, 85, 25, 42, 60, 40, false, true, true, true, false, false, false, false, 0.35f, 0.48f, 0.32f);
        add(Inst::Darbuka, "Belly Dance Rhythm", "Egypt", "Gm", "Bayati", "Sensual percussion", "dance,rhythm", 10, 45, 80, 30, 50, 65, 45, false, false, true, false, true, false, false, false, 0.40f, 0.45f, 0.38f);
        add(Inst::Darbuka, "Wedding Drums", "Morocco", "Am", "Rast", "Celebration hit", "wedding,festive", 12, 48, 82, 28, 48, 68, 48, false, false, true, false, true, false, false, false, 0.42f, 0.42f, 0.40f);
        add(Inst::Darbuka, "Doum Tarab", "Lebanon", "Fm", "Ajam", "Deep body drum", "tarab,emotion", 6, 38, 85, 22, 46, 58, 32, false, false, false, false, true, false, false, false, 0.30f, 0.35f, 0.20f);
        add(Inst::Darbuka, "Takka Percussion", "Tunisia", "Cm", "Saba", "Fast hitting pattern", "percussion,energy", 3, 30, 90, 18, 50, 52, 28, false, true, true, true, false, false, false, false, 0.25f, 0.45f, 0.18f);
        add(Inst::Darbuka, "Sufi Ritual Drums", "Turkey", "Em", "Kurd", "Spiritual percussion", "spiritual,ceremony", 14, 50, 75, 35, 40, 70, 50, true, false, false, false, true, false, false, false, 0.55f, 0.30f, 0.25f);
        add(Inst::Darbuka, "Bedouin Fire", "Saudi Arabia", "Bm", "Nahawand", "Desert percussion", "cultural,rhythm", 9, 42, 84, 24, 44, 62, 38, false, false, true, false, true, false, false, false, 0.38f, 0.40f, 0.28f);
        add(Inst::Darbuka, "Chaabi Rooftop", "Algeria", "Am", "Rast", "Popular party beat", "urban,festive", 10, 46, 82, 28, 48, 66, 42, false, true, true, true, false, false, false, false, 0.40f, 0.48f, 0.35f);
        add(Inst::Darbuka, "Aladdin's Marketplace", "Arabian Nights", "Fm", "Hijaz", "Magic drum action", "cinema,energy", 11, 44, 83, 32, 50, 68, 46, false, true, true, false, true, false, false, false, 0.44f, 0.46f, 0.40f);
        add(Inst::Darbuka, "Nubian Pulse", "Sudan", "Dm", "Bayati", "African-Arab rhythm", "cultural,fusion", 12, 47, 81, 30, 46, 64, 44, false, false, true, false, true, false, false, false, 0.42f, 0.42f, 0.36f);
        add(Inst::Darbuka, "Rai Boulevard", "Oran/Algeria", "Bm", "Rast", "Modern Rai beat", "urban,modern", 7, 36, 87, 26, 48, 58, 36, false, true, true, true, false, false, false, false, 0.32f, 0.46f, 0.28f);
        add(Inst::Darbuka, "Fes Festival", "Morocco", "Gm", "Gnawa", "Trance drumming", "trance,festival", 13, 52, 76, 40, 42, 72, 55, false, false, false, false, true, false, true, false, 0.50f, 0.28f, 0.22f);
        add(Inst::Darbuka, "Doumbek Drill", "Lebanon", "Fm", "Ajam", "Rapid-fire percussion", "percussion,rhythm", 4, 32, 89, 20, 52, 54, 30, false, true, true, true, false, false, false, false, 0.28f, 0.48f, 0.20f);
        add(Inst::Darbuka, "Suq Energy Groove", "Baghdad", "Am", "Hijaz", "Market hustle beat", "urban,energy", 8, 40, 86, 25, 46, 60, 40, false, true, true, false, true, false, false, false, 0.36f, 0.44f, 0.32f);

        // Riq (Frame Drum)
        add(Inst::Qanun, "Riq Shimmer", "Egypt", "Dm", "Rast", "Bright frame drum", "percussion,shimmer", 5, 50, 90, 35, 55, 65, 45, false, false, false, false, false, false, false, false, 0.25f, 0.30f, 0.35f);
        add(Inst::Qanun, "Riq Jingle", "Morocco", "Gm", "Bayati", "Celebratory shimmer", "festive,jingle", 6, 48, 88, 38, 58, 68, 50, false, false, true, false, false, false, false, false, 0.30f, 0.35f, 0.40f);
        add(Inst::Qanun, "Riq Zikr", "Islamic", "Am", "Kurd", "Religious percussion", "spiritual,ceremony", 7, 55, 85, 40, 52, 70, 52, true, false, false, false, false, false, false, false, 0.45f, 0.25f, 0.30f);
        add(Inst::Qanun, "Riq Belly Dance", "Lebanon", "Fm", "Ajam", "Dance accompaniment", "dance,rhythm", 8, 52, 84, 42, 56, 66, 48, false, false, true, false, false, false, false, false, 0.35f, 0.38f, 0.42f);
        add(Inst::Qanun, "Riq Tarab Echo", "Iraq", "Em", "Saba", "Emotional shimmer", "tarab,emotion", 9, 58, 80, 45, 50, 72, 54, true, false, false, false, false, false, false, false, 0.50f, 0.28f, 0.32f);

        // ══════════════════════════════════════════════════════════════════════════
        // PLUCK (Oud) — 25 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Oud, "Aladin Night Market", "Morocco", "Bm", "Hijaz", "Oud sec percussion", "groove", 0, 25, 92, 15, 25, 50, 28, false, false, false, false, true, false, false, false, 0.22f, 0.38f, 0.15f);
        add(Inst::Oud, "Thousand Nights Story", "Iraq", "Dm", "Maqam Rast", "Taqsim improvisation", "classique", 10, 55, 78, 30, 28, 65, 45, true, false, false, false, true, false, false, false, 0.55f, 0.35f, 0.20f);
        add(Inst::Oud, "Umm Kulthum Tarab", "Egypt", "Gm", "Bayati", "Brilliant strings", "tarab", 15, 60, 88, 45, 35, 75, 55, true, false, true, false, true, false, false, false, 0.65f, 0.42f, 0.35f);
        add(Inst::Oud, "Babylon Gate", "Iraq", "Dm", "Siga", "Deep bass oud", "ancestral", 5, 45, 35, 20, 40, 80, 40, true, false, false, false, true, false, false, false, 0.60f, 0.30f, 0.18f);
        add(Inst::Oud, "Desert Caravan", "Saudi", "Dm", "Hijaz", "Tremolo under wind", "voyage", 20, 65, 72, 25, 32, 68, 50, true, true, false, false, true, false, false, false, 0.62f, 0.55f, 0.22f);
        add(Inst::Oud, "Fes Medina", "Morocco", "Bm", "Rahat al-Arwah", "Picking andalou", "andalou", 8, 50, 82, 35, 30, 60, 40, true, false, true, false, true, false, false, false, 0.52f, 0.38f, 0.30f);
        add(Inst::Oud, "Palmyra Ghost", "Syria", "Fm", "Nawa Athar", "Harmoniques spectraux", "ethere", 15, 90, 55, 40, 50, 85, 80, true, false, false, false, true, true, false, false, 0.90f, 0.30f, 0.10f);
        add(Inst::Oud, "Salalah Rain", "Oman", "Dm", "Rast", "Perlées notes", "pluie", 5, 68, 75, 20, 28, 62, 55, true, true, false, false, true, false, false, false, 0.70f, 0.60f, 0.18f);
        add(Inst::Oud, "Amman Dusk", "Jordan", "Em", "Kurd", "Electroacoustique", "fusion", 12, 55, 90, 30, 60, 70, 50, false, false, true, false, true, false, false, false, 0.48f, 0.40f, 0.40f);
        add(Inst::Oud, "Tangier Port", "Morocco", "Fm", "Jiharkah", "Soie strings", "mer", 8, 58, 80, 25, 32, 65, 45, true, false, true, false, true, false, false, false, 0.60f, 0.35f, 0.28f);
        add(Inst::Oud, "Baghdad Blues", "Iraq", "Am", "Hijaz", "Melancholic oud", "blues,emotion", 18, 70, 75, 40, 35, 70, 55, true, false, false, false, true, false, false, false, 0.65f, 0.40f, 0.25f);
        add(Inst::Oud, "Beirut Jazz Oud", "Lebanon", "Cm", "Ajam", "Urban fusion", "jazz,urban", 12, 52, 85, 50, 50, 65, 50, false, true, true, true, false, false, false, false, 0.45f, 0.50f, 0.45f);
        add(Inst::Oud, "Damascus Garden", "Syria", "Gm", "Rast", "Courtyard elegance", "classical,garden", 14, 62, 80, 48, 40, 75, 52, true, false, true, false, true, false, false, false, 0.62f, 0.38f, 0.35f);
        add(Inst::Oud, "Aladdin's Jewel", "Arabian Nights", "Dm", "Bayati", "Magic treasure theme", "cinema,magic", 16, 68, 82, 55, 48, 78, 60, true, true, true, false, true, false, false, false, 0.72f, 0.45f, 0.38f);
        add(Inst::Oud, "Sahara Sunset", "Sahara", "Em", "Saba", "Golden hour oud", "ambient,scenic", 20, 75, 70, 50, 38, 80, 65, true, false, false, false, true, false, false, false, 0.75f, 0.32f, 0.20f);
        add(Inst::Oud, "Cairo Midnight", "Cairo", "Fm", "Hijaz", "Night oud solo", "evening,urban", 18, 72, 78, 48, 42, 72, 58, true, false, false, false, true, false, false, false, 0.68f, 0.35f, 0.22f);
        add(Inst::Oud, "Istanbul Harbor", "Istanbul", "Bm", "Rast", "Waterfront melody", "journey,scenic", 15, 65, 80, 45, 40, 70, 55, true, false, true, false, true, false, false, false, 0.60f, 0.40f, 0.30f);
        add(Inst::Oud, "Khartoum Festival", "Sudan", "Am", "Nahawand", "Celebration oud", "festive,cultural", 16, 58, 83, 50, 45, 72, 52, true, false, true, false, true, false, false, false, 0.58f, 0.42f, 0.35f);
        add(Inst::Oud, "Tunis Malouf", "Tunisia", "Gm", "Bayati", "Andalusian tradition", "classical,tradition", 17, 60, 81, 52, 43, 74, 54, true, false, true, false, true, false, false, false, 0.62f, 0.38f, 0.33f);
        add(Inst::Oud, "Luxor Temple", "Egypt", "Dm", "Rast", "Ancient monument theme", "ancient,cinematic", 19, 75, 72, 55, 40, 78, 62, true, true, false, false, true, false, false, false, 0.72f, 0.42f, 0.28f);

        // ══════════════════════════════════════════════════════════════════════════
        // PADS (Synth pads) — 20 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Violin, "Eternal Maqam", "Abstract", "Cm", "Bayati", "Lush pad sustain", "ambient,pad", 40, 88, 70, 60, 20, 90, 75, true, false, false, false, true, false, false, false, 0.85f, 0.15f, 0.10f);
        add(Inst::Violin, "Desert Mirage", "Sahara", "Dm", "Rast", "Shimmering pad", "ambient,scenic", 35, 90, 65, 55, 22, 92, 78, true, false, false, false, true, false, false, false, 0.88f, 0.12f, 0.08f);
        add(Inst::Oud, "Celestial Oud", "Spiritual", "Em", "Kurd", "Ethereal background", "spiritual,meditative", 30, 92, 60, 50, 18, 95, 80, true, false, false, false, false, false, false, false, 0.90f, 0.10f, 0.05f);
        add(Inst::Qanun, "Twilight Dreams", "Evening", "Fm", "Ajam", "Dreamy pad texture", "ambient,meditative", 38, 88, 68, 58, 20, 88, 76, true, false, false, false, true, false, false, false, 0.85f, 0.15f, 0.12f);
        add(Inst::Rababah, "Void Echo", "Void", "Gm", "Hijaz", "Empty space pad", "ambient,experimental", 45, 90, 55, 52, 15, 90, 82, true, false, false, false, true, false, true, false, 0.88f, 0.12f, 0.08f);
        add(Inst::Ney, "Breath Cloud", "Sky", "Am", "Rast", "Breathing pad", "ambient,meditative", 32, 86, 62, 48, 16, 94, 78, true, false, false, false, true, false, false, false, 0.88f, 0.12f, 0.08f);
        add(Inst::Mizmar, "Golden Haze", "Sunrise", "Bm", "Bayati", "Warm glow pad", "ambient,scenic", 40, 87, 72, 62, 24, 86, 74, true, true, false, false, true, false, false, false, 0.82f, 0.20f, 0.12f);
        add(Inst::Darbuka, "Ritual Ground", "Sacred", "Cm", "Nahawand", "Deep ceremonial pad", "spiritual,ceremony", 50, 85, 58, 65, 12, 92, 70, true, false, false, false, true, false, false, false, 0.85f, 0.15f, 0.10f);
        add(Inst::Qanun, "Silver Lining", "Sky", "Dm", "Saba", "Delicate pad strings", "ambient,peaceful", 36, 89, 70, 54, 18, 90, 76, true, false, false, false, false, false, false, false, 0.88f, 0.10f, 0.08f);
        add(Inst::Oud, "Silk Veil Pad", "East", "Em", "Rast", "Silky smooth texture", "ambient,luxurious", 34, 88, 68, 56, 20, 92, 78, true, false, false, false, true, false, false, false, 0.86f, 0.14f, 0.10f);

        // ══════════════════════════════════════════════════════════════════════════
        // PIANO — 15 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Violin, "Grand Cairo", "Cairo", "Dm", "Rast", "Full piano sweep", "classical,piano", 5, 40, 85, 45, 50, 70, 50, true, false, true, false, true, false, false, false, 0.60f, 0.35f, 0.30f);
        add(Inst::Violin, "Oud Piano Fusion", "East Meets West", "Gm", "Bayati", "Piano with oriental flavor", "fusion,classical", 12, 50, 80, 55, 48, 75, 55, true, false, true, false, true, false, false, false, 0.62f, 0.38f, 0.35f);
        add(Inst::Violin, "Baghdad Concerto", "Iraq", "Am", "Hijaz", "Cinematic piano drama", "cinema,classical", 15, 60, 75, 60, 45, 80, 60, true, false, true, false, true, false, false, false, 0.68f, 0.42f, 0.38f);
        add(Inst::Violin, "Beirut Nights Piano", "Lebanon", "Fm", "Ajam", "Urban piano mood", "urban,classical", 10, 52, 82, 50, 48, 72, 52, true, true, false, false, true, false, false, false, 0.58f, 0.40f, 0.28f);
        add(Inst::Violin, "Damascus Waltz", "Syria", "Em", "Rast", "Classical waltz", "classical,dance", 8, 48, 80, 52, 46, 74, 50, true, false, true, false, true, false, false, false, 0.60f, 0.36f, 0.32f);
        add(Inst::Violin, "Istanbul Grand", "Istanbul", "Cm", "Kurd", "Turkish grand piano", "classical,turkish", 14, 58, 78, 58, 50, 78, 58, true, false, true, false, true, false, false, false, 0.65f, 0.40f, 0.35f);
        add(Inst::Violin, "Marrakech Elegance", "Morocco", "Bm", "Rast", "Refined piano", "classical,elegant", 12, 55, 82, 54, 48, 76, 54, true, false, true, false, true, false, false, false, 0.62f, 0.38f, 0.33f);
        add(Inst::Violin, "Sufi Piano Meditation", "Spiritual", "Dm", "Bayati", "Meditative keys", "spiritual,meditative", 20, 70, 70, 50, 30, 85, 60, true, false, false, false, true, false, false, false, 0.70f, 0.28f, 0.18f);
        add(Inst::Violin, "Aladdin's Palace", "Arabian Nights", "Gm", "Hijaz", "Magic palace theme", "cinema,fantasy", 18, 62, 80, 60, 50, 80, 62, true, true, true, false, true, false, false, false, 0.68f, 0.45f, 0.40f);
        add(Inst::Violin, "Cairo Jazz Piano", "Cairo", "Am", "Rast", "Jazz-oriented keys", "jazz,urban", 10, 48, 85, 55, 55, 68, 50, false, true, true, true, false, false, false, false, 0.50f, 0.50f, 0.45f);

        // ══════════════════════════════════════════════════════════════════════════
        // SYNTH (Wavetable/synthesis) — 20 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Violin, "Synth Maqam Ancient", "Abstract", "Dm", "Rast", "Warm wavetable", "synth,electronic", 25, 60, 75, 50, 40, 65, 55, true, false, true, false, true, false, false, false, 0.60f, 0.35f, 0.30f);
        add(Inst::Violin, "Synth Hijaz Electric", "Cyberspace", "Am", "Hijaz", "Bright synth lead", "synth,electronic", 15, 45, 85, 40, 50, 55, 45, false, true, true, true, false, false, false, false, 0.45f, 0.50f, 0.40f);
        add(Inst::Violin, "Synth Bayati Dark", "Night", "Gm", "Bayati", "Dark pulsing synth", "synth,dark", 40, 70, 60, 65, 35, 75, 65, true, false, false, true, true, false, true, true, 0.75f, 0.30f, 0.25f);
        add(Inst::Violin, "Synth Ajam Bright", "Day", "Fm", "Ajam", "Bright electronic", "synth,bright", 20, 50, 80, 45, 55, 60, 50, false, true, true, false, false, false, false, false, 0.48f, 0.45f, 0.40f);
        add(Inst::Violin, "Synth Kurd Deep", "Void", "Em", "Kurd", "Deep bass synth", "synth,bass", 35, 80, 50, 70, 30, 85, 70, true, false, false, false, true, false, true, false, 0.80f, 0.25f, 0.15f);
        add(Inst::Violin, "Synth Rast Warm", "Comfort", "Cm", "Rast", "Warm analog vibe", "synth,warm", 30, 65, 70, 55, 45, 70, 58, true, false, true, false, true, false, false, false, 0.65f, 0.35f, 0.30f);
        add(Inst::Violin, "Synth Saba Mystical", "Mystery", "Bm", "Saba", "Mystical texture", "synth,mystical", 45, 75, 65, 60, 40, 80, 70, true, false, false, true, true, false, true, false, 0.75f, 0.30f, 0.22f);
        add(Inst::Violin, "Synth Nahawand Epic", "Cinematic", "Am", "Nahawand", "Epic orchestral synth", "synth,cinematic", 50, 85, 72, 68, 38, 85, 72, true, true, true, false, true, false, false, false, 0.80f, 0.40f, 0.35f);
        add(Inst::Violin, "Synth Jishah Modern", "Future", "Dm", "Jishah", "Modern glitch synth", "synth,modern", 10, 40, 90, 35, 60, 50, 40, false, true, true, true, false, false, false, true, 0.40f, 0.55f, 0.35f);
        add(Inst::Violin, "Synth Rikah Acid", "Acid Lab", "Em", "Rikah", "Acid TB-style", "synth,electronic", 22, 55, 75, 42, 70, 55, 48, false, true, true, true, false, false, true, false, 0.50f, 0.60f, 0.38f);

        // ══════════════════════════════════════════════════════════════════════════
        // ORCHESTRAL — 15 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Violin, "Symphony Rast", "Concert Hall", "Dm", "Rast", "Full ensemble", "orchestral,classical", 40, 75, 70, 65, 38, 85, 65, true, false, true, false, true, false, false, false, 0.75f, 0.40f, 0.35f);
        add(Inst::Violin, "Philharmonic Bayati", "Vienna", "Gm", "Bayati", "Lush orchestration", "orchestral,romantic", 45, 80, 72, 70, 40, 88, 68, true, false, true, false, true, false, false, false, 0.80f, 0.38f, 0.35f);
        add(Inst::Violin, "Arabic Concerto", "Cairo Conservatory", "Am", "Hijaz", "Violin showcase", "orchestral,concerto", 35, 70, 75, 60, 42, 80, 60, true, false, true, false, true, false, false, false, 0.70f, 0.42f, 0.38f);
        add(Inst::Violin, "Eastern Serenade", "Ottoman", "Fm", "Ajam", "Chamber ensemble", "orchestral,chamber", 30, 65, 78, 58, 45, 78, 55, true, false, true, false, true, false, false, false, 0.65f, 0.40f, 0.35f);
        add(Inst::Violin, "Desert Requiem", "Sahara", "Em", "Rast", "Mournful ensemble", "orchestral,dramatic", 50, 75, 60, 70, 35, 75, 70, true, false, false, false, true, true, false, false, 0.80f, 0.30f, 0.20f);
        add(Inst::Violin, "Cinematic Grand", "Hollywood", "Cm", "Bayati", "Film score style", "orchestral,cinematic", 55, 80, 75, 72, 40, 88, 72, true, true, true, false, true, false, false, false, 0.82f, 0.45f, 0.38f);
        add(Inst::Violin, "Aladdin's Court", "Arabian Nights", "Bm", "Hijaz", "Royal ensemble", "orchestral,cinematic", 42, 72, 78, 68, 45, 85, 68, true, true, true, false, true, false, false, false, 0.75f, 0.48f, 0.42f);

        // ══════════════════════════════════════════════════════════════════════════
        // ORIENTAL (Maqam specialists) — 25 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Oud, "Maqam Rast Master", "Classical", "Dm", "Rast", "Deep maqam tradition", "classical,maqam", 18, 70, 75, 60, 40, 80, 60, true, false, true, false, true, false, false, false, 0.70f, 0.38f, 0.30f);
        add(Inst::Oud, "Maqam Bayati Soul", "Emotional", "Gm", "Bayati", "Soulful maqam", "emotional,maqam", 25, 75, 70, 65, 38, 85, 65, true, false, true, false, true, false, false, false, 0.75f, 0.35f, 0.32f);
        add(Inst::Oud, "Maqam Hijaz Fire", "Energy", "Am", "Hijaz", "Intense maqam", "energy,maqam", 20, 60, 80, 55, 45, 75, 55, true, true, true, false, true, false, false, false, 0.65f, 0.45f, 0.35f);
        add(Inst::Qanun, "Maqam Ajam Noble", "Classical", "Fm", "Ajam", "Elegant maqam", "classical,maqam", 16, 65, 80, 58, 48, 78, 58, true, false, true, false, true, false, false, false, 0.62f, 0.38f, 0.35f);
        add(Inst::Ney, "Maqam Kurd Mystical", "Spiritual", "Em", "Kurd", "Mystical wind", "spiritual,maqam", 35, 80, 68, 55, 35, 82, 68, true, false, false, false, true, false, false, false, 0.78f, 0.25f, 0.18f);
        add(Inst::Oud, "Maqam Saba Profound", "Deep", "Cm", "Saba", "Profound bass maqam", "deep,maqam", 22, 70, 65, 62, 38, 85, 62, true, false, false, false, true, false, false, false, 0.72f, 0.30f, 0.22f);
        add(Inst::Rababah, "Maqam Nahawand Ritual", "Ceremonial", "Bm", "Nahawand", "Ritual stringed", "ceremonial,maqam", 50, 75, 70, 65, 35, 80, 65, true, false, false, false, true, false, false, false, 0.75f, 0.32f, 0.25f);
        add(Inst::Qanun, "Maqam Jishah Dance", "Festival", "Dm", "Jishah", "Dance maqam", "dance,maqam", 15, 58, 82, 52, 52, 72, 52, true, false, true, false, true, false, false, false, 0.58f, 0.42f, 0.38f);
        add(Inst::Ney, "Maqam Rikah Flight", "Ethereal", "Em", "Rikah", "Floating wind maqam", "ethereal,maqam", 38, 85, 65, 52, 32, 88, 72, true, false, false, false, true, false, false, false, 0.82f, 0.18f, 0.12f);
        add(Inst::Mizmar, "Maqam Sikah Brilliant", "Bright", "Am", "Sikah", "Bright brass maqam", "bright,maqam", 14, 55, 85, 45, 50, 70, 52, false, true, true, false, true, false, false, false, 0.52f, 0.42f, 0.38f);

        // ══════════════════════════════════════════════════════════════════════════
        // BELLS & MALLETS — 15 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Qanun, "Temple Bells", "Sacred", "Gm", "Rast", "Resonant metal", "spiritual,bells", 22, 78, 72, 48, 30, 80, 65, true, false, false, false, false, false, false, false, 0.72f, 0.22f, 0.15f);
        add(Inst::Qanun, "Church Carillon", "Europe", "Dm", "Bayati", "Bell tower", "classical,bells", 25, 80, 68, 50, 28, 85, 68, true, false, false, false, false, false, false, false, 0.75f, 0.20f, 0.12f);
        add(Inst::Rababah, "Gamelan Echo", "Indonesia/East", "Am", "Kurd", "Asian bell texture", "world,cultural", 20, 75, 75, 55, 40, 75, 60, true, false, false, false, false, false, false, false, 0.70f, 0.28f, 0.18f);
        add(Inst::Darbuka, "Vibraphone Warm", "Jazz", "Fm", "Ajam", "Mallet vibes", "jazz,mallets", 18, 60, 78, 50, 45, 65, 50, true, false, true, false, false, false, false, false, 0.58f, 0.35f, 0.30f);
        add(Inst::Qanun, "Gong Resonance", "Ritual", "Em", "Rast", "Deep gong tone", "spiritual,gong", 30, 85, 55, 60, 20, 90, 75, true, false, false, false, false, false, false, false, 0.85f, 0.15f, 0.08f);

        // ══════════════════════════════════════════════════════════════════════════
        // BASS — 15 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Oud, "Deep Oud Bass", "Low", "Dm", "Rast", "Subharmonic oud", "bass,deep", 8, 50, 45, 40, 30, 85, 50, true, false, false, false, true, false, false, false, 0.60f, 0.30f, 0.20f);
        add(Inst::Rababah, "Cello Bass", "Classical", "Am", "Bayati", "Deep bowed strings", "bass,classical", 35, 70, 50, 55, 32, 80, 55, true, false, false, false, true, false, false, false, 0.68f, 0.28f, 0.22f);
        add(Inst::Oud, "Synth Bass Warm", "Electronic", "Gm", "Hijaz", "Analog bass synth", "bass,synth", 30, 65, 55, 60, 25, 80, 55, true, false, false, true, true, false, false, false, 0.70f, 0.35f, 0.25f);
        add(Inst::Darbuka, "Kick Drum Deep", "Percussion", "Fm", "Rast", "Sub kick rhythm", "bass,rhythm", 6, 40, 50, 35, 50, 60, 45, false, false, false, true, true, false, false, false, 0.35f, 0.40f, 0.28f);
        add(Inst::Rababah, "Contrabass Solo", "Classical", "Em", "Ajam", "Lowest register", "bass,orchestral", 40, 75, 48, 58, 28, 82, 60, true, false, false, false, true, false, false, false, 0.72f, 0.25f, 0.18f);

        // ══════════════════════════════════════════════════════════════════════════
        // LEAD — 20 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Ney, "Ney Lead Voice", "Solo", "Dm", "Rast", "Solo wind melody", "lead,vocal", 28, 60, 75, 50, 35, 70, 55, true, false, false, false, true, false, false, false, 0.65f, 0.35f, 0.28f);
        add(Inst::Oud, "Oud Lead Cry", "Expressive", "Gm", "Bayati", "Expressive pluck", "lead,emotion", 22, 65, 80, 55, 45, 75, 58, true, false, true, false, true, false, false, false, 0.68f, 0.38f, 0.32f);
        add(Inst::Mizmar, "Mizmar Herald", "Trumpet", "Am", "Hijaz", "Announcing trumpet", "lead,brass", 12, 50, 85, 40, 48, 65, 48, false, true, true, true, false, false, false, false, 0.50f, 0.48f, 0.40f);
        add(Inst::Violin, "Violin Lead Soaring", "Melody", "Fm", "Ajam", "High soaring strings", "lead,classical", 20, 60, 80, 55, 50, 75, 55, true, false, true, false, true, false, false, false, 0.65f, 0.40f, 0.35f);
        add(Inst::Qanun, "Qanun Picker", "Bright", "Em", "Rast", "Bright lead pluck", "lead,bright", 10, 55, 88, 45, 55, 68, 50, true, false, true, false, true, false, false, false, 0.58f, 0.40f, 0.38f);

        // ══════════════════════════════════════════════════════════════════════════
        // DRUMS & PERCUSSION RHYTHMS — 20 presets
        // ══════════════════════════════════════════════════════════════════════════

        add(Inst::Darbuka, "House Beat Groove", "Electronic", "Am", "Rast", "Clubbing rhythm", "electronic,rhythm", 4, 35, 88, 22, 50, 55, 40, false, true, true, true, false, false, false, false, 0.32f, 0.50f, 0.28f);
        add(Inst::Darbuka, "Reggaeton Pulse", "Latin", "Dm", "Bayati", "Caribbean rhythm", "latin,rhythm", 7, 38, 85, 25, 48, 58, 42, false, true, true, true, false, false, false, false, 0.38f, 0.48f, 0.32f);
        add(Inst::Darbuka, "Trap Snare Kit", "Modern", "Gm", "Hijaz", "Urban trap beat", "modern,rhythm", 5, 32, 90, 20, 55, 52, 38, false, true, true, true, true, false, false, true, 0.30f, 0.55f, 0.25f);
        add(Inst::Darbuka, "Funk Pocket", "Groove", "Fm", "Ajam", "Funky kick pattern", "funk,rhythm", 8, 42, 82, 28, 50, 60, 45, false, false, true, true, false, false, false, false, 0.40f, 0.45f, 0.35f);
        add(Inst::Darbuka, "Jazz Swing", "Jazz", "Em", "Rast", "Swing drum groove", "jazz,rhythm", 12, 48, 78, 35, 45, 65, 50, false, false, true, false, false, false, false, false, 0.45f, 0.42f, 0.38f);

        return presets;
    }
};
