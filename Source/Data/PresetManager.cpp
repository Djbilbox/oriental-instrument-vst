#include "PresetManager.h"
#include "InstrumentProfiles.h"
#include "PresetData_Extended.h"

PresetManager::PresetManager()
{
    loadFactoryPresets();
    loadExtendedPresets();

#ifdef ORIENTAL_DEMO
    // DEMO build: keep only the first 5 presets of each instrument (lighter demo).
    {
        std::vector<PresetData> demo;
        for (int inst = 0; inst < 7; ++inst)
        {
            int kept = 0;
            for (const auto& p : factoryPresets)
            {
                if (static_cast<int>(p.instrument) != inst) continue;
                if (kept++ >= 5) break;
                demo.push_back(p);
            }
        }
        factoryPresets.swap(demo);
    }
#endif
}

// Derive a DISTINCT ADSR per preset from its own macro personality, anchored on
// the instrument's base envelope. Previously every preset of an instrument got
// the identical profile ADSR → all 10 sounded the same. Now glide lengthens the
// attack (legato vs pluck), space stretches the release (long reverb tails),
// depth feeds sustain (richer = fuller body), filter shapes the decay.
static void deriveADSR(PresetData& p)
{
    const auto& prof = InstrumentProfiles::getProfile(p.instrument);
    const float g  = p.glide  / 100.0f;
    const float sp = p.space  / 100.0f;
    const float fl = p.filter / 100.0f;
    const float de = p.depth  / 100.0f;

    p.attack  = juce::jlimit(0.001f, 2.5f, prof.attack  * (0.45f + g  * 2.2f));
    p.decay   = juce::jlimit(0.01f,  3.0f, prof.decay   * (0.65f + (1.0f - fl) * 0.9f));
    p.sustain = juce::jlimit(0.0f,   1.0f, prof.sustain * (0.55f + de * 0.7f));
    p.release = juce::jlimit(0.01f,  4.0f, prof.release * (0.55f + sp * 1.7f));
}

void PresetManager::loadFactoryPresets()
{
    factoryPresets.clear();
    factoryPresets.reserve(OrientalConstants::TOTAL_PRESETS);

    using Inst = OrientalConstants::Instrument;

    // Lambda complet avec tous les paramètres sonores uniques par preset
    // Params: inst, name, city, key, maqam, desc, tags,
    //         glide, space, filter, orient, reso, depth, fxMix,
    //         reverbOn, echoOn, chorusOn, distOn, compOn, phaserOn,
    //         reverbAmt, echoAmt, chorusAmt
    auto add = [&](Inst inst,
                   const char* name, const char* city, const char* key, const char* maqam,
                   const char* sound, const char* tags,
                   float glide, float space, float filter, float orient,
                   float reso,  float depth, float fxMix,
                   bool revOn, bool echoOn, bool choOn, bool distOn, bool compOn, bool phasOn,
                   float revAmt, float echoAmt, float choAmt)
    {
        PresetData p;
        p.instrument      = inst;
        p.name            = name;  p.city = city;  p.key = key;  p.maqam = maqam;
        p.soundDescription = sound; p.tags = tags;
        p.glide  = glide;  p.space  = space;  p.filter = filter;
        p.orient = orient; p.reso   = reso;   p.depth  = depth;  p.fxMix = fxMix;
        p.reverbOn     = revOn;  p.echoOn   = echoOn; p.chorusOn = choOn;
        p.distortOn    = distOn; p.compressorOn = compOn; p.phaserOn = phasOn;
        p.reverbAmount = revAmt; p.echoAmount = echoAmt; p.chorusAmount = choAmt;
        deriveADSR(p);
        factoryPresets.push_back(p);
    };

    // ══ VIOLIN (0-9) ═══════════════════════════════════════════════════════════
    //                                          gl   sp   fi   or   re   de   fx   rOn ecOn chOn diOn coOn phOn  rAmt ecAmt chAmt
    add(Inst::Violin,"Mission Cleopatre",    "Cairo",  "Dm","Rast",    "Legato strings warm, vibrato egyptien","cinema",
        35,  70,  75,  65,  40,  80,  60,   true, false, true, false,  true,false, 0.78f,0.42f,0.32f);
    add(Inst::Violin,"Desert Cinema Score",  "Egypt",  "Am","Hijaz",   "Tremolo sul ponticello, tension drama","score",
        18,  50,  60,  50,  55,  65,  45,   true, false,false,false,  true,false, 0.55f,0.30f,0.18f);
    add(Inst::Violin,"Casablanca Nights",    "Morocco","Fm","Saba",    "Pizzicato jazz oriental, swing arabe","jazz",
         5,  45,  88,  25,  30,  55,  38,  false, false, true,false,  true,false, 0.40f,0.45f,0.42f);
    add(Inst::Violin,"Atlas Tears",          "Algeria","Bm","Kurd",    "Harmonique flageolet, larme berbere","emotion",
        40,  75,  70,  80,  35,  85,  65,   true, false, true,false,  true,false, 0.72f,0.38f,0.28f);
    add(Inst::Violin,"Beirut Lovers",        "Lebanon","Cm","Ajam",    "Double corde romantique, vibrato lent","amour",
        45,  65,  72,  70,  38,  82,  55,   true, false, true,false,  true,false, 0.68f,0.35f,0.35f);
    add(Inst::Violin,"Oran Sunset Strings",  "Algeria","Em","Nahawand","Ensemble cordes, coucher de soleil","ambient",
        30,  85,  68,  55,  32,  88,  70,   true,  true, true,false,  true,false, 0.80f,0.50f,0.40f);
    add(Inst::Violin,"Chaabi Wedding Strings","Morocco","Am","Rast",   "Staccato festif, ornements chaabi","festif",
         8,  40,  88,  35,  42,  65,  35,  false, false, true,false,  true,false, 0.35f,0.40f,0.45f);
    add(Inst::Violin,"Midnight Desert Scene","Libya",  "Dm","Hijaz Kar","Solo lent sur fond pad sombre","mystere",
        50,  80,  45,  60,  45,  72,  72,   true, false,false,false,  true, true, 0.85f,0.28f,0.15f);
    add(Inst::Violin,"Oriental Drama",       "Jordan", "Fm","Saba Zamzam","Tutti cordes dramatique, fff","drama",
        25,  60,  80,  75,  50,  90,  55,   true, false, true,false,  true,false, 0.75f,0.45f,0.38f);
    add(Inst::Violin,"Aleppo Lament",        "Syria",  "Gm","Bayati",  "Solo plaintif, portamento douloureux","nostalgie",
        55,  72,  58,  85,  42,  78,  60,   true, false,false,false,  true,false, 0.82f,0.25f,0.12f);

    // ══ OUD (10-19) ════════════════════════════════════════════════════════════
    add(Inst::Oud,"Aladin Night Market",   "Morocco","Bm","Hijaz",      "Oud sec et percussif, attaque claquee","groove",
         0,  25,  92,  15,  25,  50,  28,  false, false,false,false,  true,false, 0.22f,0.38f,0.15f);
    add(Inst::Oud,"Thousand Nights Story", "Iraq",   "Dm","Maqam Rast", "Taqsim improvisation pur, libre","classique",
        10,  55,  78,  30,  28,  65,  45,   true, false,false,false,  true,false, 0.55f,0.35f,0.20f);
    add(Inst::Oud,"Umm Kulthum Tribute",   "Egypt",  "Gm","Bayati",     "Cordes metalliques brillantes, tarab","tarab",
        15,  60,  88,  45,  35,  75,  55,   true, false, true,false,  true,false, 0.65f,0.42f,0.35f);
    add(Inst::Oud,"Babylon Gate",          "Iraq",   "Dm","Siga",       "Oud basse profonde, grave mesopotamien","ancestral",
         5,  45,  35,  20,  40,  80,  40,   true, false,false,false,  true,false, 0.60f,0.30f,0.18f);
    add(Inst::Oud,"Desert Caravan",        "Saudi",  "Dm","Hijaz",      "Tremolo oud sous le vent de sable","voyage",
        20,  65,  72,  25,  32,  68,  50,   true,  true,false,false,  true,false, 0.62f,0.55f,0.22f);
    add(Inst::Oud,"Fes Medina",            "Morocco","Bm","Rahat al-Arwah","Picking andalou raffine, medina","andalou",
         8,  50,  82,  35,  30,  60,  40,   true, false, true,false,  true,false, 0.52f,0.38f,0.30f);
    add(Inst::Oud,"Palmyra Ghost",         "Syria",  "Fm","Nawa Athar", "Harmoniques spectraux, reverbere","ethere",
        15,  90,  55,  40,  50,  85,  80,   true, false,false,false,  true, true, 0.90f,0.30f,0.10f);
    add(Inst::Oud,"Salalah Rain",          "Oman",   "Dm","Rast",       "Notes perlees sous la pluie d'Oman","pluie",
         5,  68,  75,  20,  28,  62,  55,   true,  true,false,false,  true,false, 0.70f,0.60f,0.18f);
    add(Inst::Oud,"Amman Dusk",            "Jordan", "Em","Kurd",       "Oud electroacoustique, filtre ouvert","fusion",
        12,  55,  90,  30,  60,  70,  50,  false, false, true,false,  true,false, 0.48f,0.40f,0.40f);
    add(Inst::Oud,"Tangier Port",          "Morocco","Fm","Jiharkah",   "Cordes en soie, port mediterraneen","mer",
         8,  58,  80,  25,  32,  65,  45,   true, false, true,false,  true,false, 0.60f,0.35f,0.28f);

    // ══ NEY (20-29) ════════════════════════════════════════════════════════════
    add(Inst::Ney,"Desert Wind Whisper",  "Algeria","Gm","Rast",      "Souffle ney quasi-flute, pp pianissimo","meditation",
        30,  80,  65,  40,  20,  75,  65,   true, false,false,false,  true,false, 0.82f,0.28f,0.12f);
    add(Inst::Ney,"Luxor Temple Wind",    "Egypt",  "Dm","Hijaz",     "Ney pharaonique grave, air chaud","temple",
        35,  75,  45,  55,  25,  80,  60,   true, false,false,false,  true,false, 0.78f,0.25f,0.15f);
    add(Inst::Ney,"Istanbul Bridge",      "Turkey", "Bm","Segah",     "Ney ottoman classique Derviche","soufi",
        40,  70,  72,  65,  28,  82,  58,   true, false, true,false,  true,false, 0.72f,0.32f,0.25f);
    add(Inst::Ney,"Wadi Rum Sunrise",     "Jordan", "Am","Rast",      "Ney solo aube dans le wadi","aube",
        38,  78,  60,  50,  22,  78,  62,   true, false,false,false,  true,false, 0.75f,0.28f,0.15f);
    add(Inst::Ney,"Medina Minarets",      "Saudi",  "Am","Saba",      "Appel spirituel, maqam saba doux","sacre",
        42,  85,  55,  45,  20,  85,  68,   true, false,false,false,  true,false, 0.85f,0.22f,0.10f);
    add(Inst::Ney,"Atlas Mountain Air",   "Morocco","Bm","Kurd",      "Air de haute altitude, ambiant 4000m","montagne",
        35,  88,  58,  35,  18,  80,  72,   true,  true,false,false, false,false, 0.88f,0.55f,0.08f);
    add(Inst::Ney,"Euphrates Breeze",     "Iraq",   "Am","Nawa Athar","Brise fluviale entre roseaux verts","fleuve",
        36,  72,  68,  52,  25,  76,  60,   true, false,false,false,  true,false, 0.76f,0.30f,0.18f);
    add(Inst::Ney,"Musandam Cliffs",      "Oman",   "Em","Nahawand",  "Echo sur falaises omaniennes","falaise",
        40,  85,  62,  48,  30,  78,  70,   true,  true,false,false,  true,false, 0.80f,0.65f,0.12f);
    add(Inst::Ney,"Carthage Ruins",       "Tunisia","Bm","Zanjaran",  "Vent dans les colonnes antiques","ruines",
        38,  90,  50,  42,  22,  82,  75,   true, false,false,false, false, true, 0.90f,0.25f,0.08f);
    add(Inst::Ney,"Sinai Whisper",        "Egypt",  "Gm","Bayati",    "Ney bedouin campement nuit etoilee","nuit",
        45,  82,  52,  58,  20,  80,  68,   true, false,false,false,  true,false, 0.84f,0.22f,0.12f);

    // ══ QANUN (30-39) ══════════════════════════════════════════════════════════
    add(Inst::Qanun,"Alexandria Elegance","Egypt",  "Gm","Hijaz Kar","Qanun crystal clair, ornements rapides","elegance",
         0,  55,  95,  25,  45,  65,  40,   true, false, true,false,  true,false, 0.52f,0.35f,0.38f);
    add(Inst::Qanun,"Damascus Ornament",  "Syria",  "Dm","Bayati",   "Mordants et trilles damascenes raffines","classique",
         0,  52,  92,  30,  42,  62,  38,   true, false, true,false,  true,false, 0.50f,0.32f,0.35f);
    add(Inst::Qanun,"Tunis Malouf",       "Tunisia","Bm","Rast",     "Malouf traditionnel, nouba tunisienne","malouf",
         5,  58,  85,  35,  38,  70,  42,   true, false, true,false,  true,false, 0.58f,0.38f,0.32f);
    add(Inst::Qanun,"Baghdad Maqam",      "Iraq",   "Em","Hijaz",    "Maqam irakien authentique, profond","maqam",
         5,  62,  75,  40,  35,  78,  45,   true, false, true,false,  true,false, 0.65f,0.40f,0.28f);
    add(Inst::Qanun,"Andalus Revival",    "Morocco","Cm","Rast",     "Musique andalouse mozarabique","andalou",
         3,  55,  80,  28,  36,  68,  38,   true, false, true,false,  true,false, 0.55f,0.35f,0.32f);
    add(Inst::Qanun,"Beirut Jazz Fusion", "Lebanon","Gm","Nahawand", "Qanun electrique jazz-oriental fusion","jazz",
         8,  48,  90,  20,  55,  60,  45,  false, false, true,false,  true,false, 0.42f,0.48f,0.48f);
    add(Inst::Qanun,"Tarab Trance",       "Egypt",  "Fm","Bayati",   "Tarab intense, emotion collective","tarab",
        10,  68,  78,  50,  40,  85,  55,   true, false, true,false,  true,false, 0.72f,0.42f,0.40f);
    add(Inst::Qanun,"Sultanate Strings",  "Oman",   "Dm","Ajam",     "Qanun royal oman, palais sultanat","royal",
         5,  65,  82,  38,  42,  75,  48,   true, false, true,false,  true,false, 0.65f,0.38f,0.35f);
    add(Inst::Qanun,"Gulf Pearl",         "Bahrain","Am","Hijaz",    "Son perle du golfe, chant des pecheurs","golfe",
         3,  60,  88,  25,  35,  62,  42,  false, false, true,false,  true,false, 0.58f,0.32f,0.38f);
    add(Inst::Qanun,"Byblos Ancient",     "Lebanon","Dm","Kurd",     "Resonances pheniciennes antiques","antique",
         5,  72,  70,  32,  38,  80,  52,   true, false,false,false,  true,false, 0.75f,0.35f,0.18f);

    // ══ DARBUKA (40-49) ════════════════════════════════════════════════════════
    add(Inst::Darbuka,"Chaabi Rooftop",    "Algeria","Dm","-","Doum-tek groovy chaabi toit d'Alger","chaabi",
         0,  35,  75,  10,  50,  55,  30,  false, false,false,false,  true,false, 0.28f,0.45f,0.18f);
    add(Inst::Darbuka,"Casbah Party",      "Algeria","Am","-","Rythme festif casbah 140bpm","fete",
         0,  30,  85,   8,  55,  50,  28,  false, false,false,false,  true,false, 0.25f,0.42f,0.15f);
    add(Inst::Darbuka,"Rai Boulevard",     "Oran",   "Bm","-","Groove rai electro-acoustique","rai",
         0,  38,  80,  12,  52,  58,  32,  false, false, true,false,  true,false, 0.30f,0.45f,0.35f);
    add(Inst::Darbuka,"Doumbek Drill",     "Lebanon","Fm","-","Tek-ka-doum ultra precis, main droite","technique",
         0,  20,  92,   5,  60,  45,  22,  false, false,false,false,  true,false, 0.18f,0.35f,0.10f);
    add(Inst::Darbuka,"Fes Festival Gnawa","Morocco","Am","-","Percussions gnawa lilas et jilala","gnawa",
         0,  42,  70,  15,  48,  62,  35,   true, false,false,false,  true,false, 0.38f,0.42f,0.20f);
    add(Inst::Darbuka,"Baghdad Funk",      "Iraq",   "Am","-","Darbuka groove funky maqam irakien","funk",
         0,  35,  82,  10,  55,  55,  30,  false, false, true,false,  true,false, 0.28f,0.48f,0.38f);
    add(Inst::Darbuka,"Nubian Pulse",      "Sudan",  "Fm","-","Pulsation nubienne chaleur intense","nubian",
         0,  40,  72,  12,  45,  65,  32,   true, false,false,false,  true,false, 0.42f,0.40f,0.18f);
    add(Inst::Darbuka,"Riyadh Trap",       "Saudi",  "Fm","-","Hi-hat trap arabe 808 orientalise","trap",
         0,  28,  88,   5,  65,  48,  25,  false, false,false, true,  true,false, 0.22f,0.38f,0.12f);
    add(Inst::Darbuka,"Tripoli Trance",    "Libya",  "Cm","-","Transe hypnotique repetitive 99bpm","transe",
         0,  55,  65,  18,  50,  70,  40,   true, false,false,false,  true, true, 0.55f,0.40f,0.22f);
    add(Inst::Darbuka,"Sousse Riddim",     "Tunisia","Fm","-","Riddim tunisien basse saturee","riddim",
         0,  32,  78,   8,  58,  52,  28,  false,  true,false,false,  true,false, 0.25f,0.52f,0.15f);

    // ══ RABABAH (50-59) ════════════════════════════════════════════════════════
    add(Inst::Rababah,"Bedouin Fire",      "Jordan", "Gm","Hijaz",    "Rababah bedouine autour du feu","feu",
        40,  58,  62,  70,  38,  72,  50,   true, false, true,false,  true,false, 0.62f,0.32f,0.28f);
    add(Inst::Rababah,"Sinai Nomad",       "Egypt",  "Em","Kurd",     "Archet long, plaine desertique vaste","nomade",
        45,  70,  55,  65,  35,  78,  55,   true, false,false,false,  true,false, 0.72f,0.28f,0.15f);
    add(Inst::Rababah,"Nejd Plains",       "Saudi",  "Cm","Hijaz Kar","Plainte des plaines du Nejd","plainte",
        48,  65,  52,  75,  40,  75,  52,   true, false,false,false,  true,false, 0.68f,0.25f,0.12f);
    add(Inst::Rababah,"Hadhramaut Valley", "Yemen",  "Cm","Bayati",   "Rababah yemenite grave et rauque","yemen",
        42,  62,  40,  60,  45,  80,  48,   true, false,false,false,  true,false, 0.65f,0.28f,0.15f);
    add(Inst::Rababah,"Palmyra Caravan",   "Syria",  "Dm","Rast",     "Melopee de caravane sous les etoiles","caravane",
        45,  72,  58,  68,  38,  72,  55,   true,  true,false,false,  true,false, 0.70f,0.52f,0.18f);
    add(Inst::Rababah,"Wahiba Sands",      "Oman",   "Am","Saba",     "Sables infinis wahiba, echo lointain","sable",
        40,  82,  50,  55,  32,  75,  62,   true, false,false,false, false, true, 0.82f,0.25f,0.10f);
    add(Inst::Rababah,"Rub al Khali Soul", "Saudi",  "Fm","Hijaz",    "Ame du quart vide, instrument unique","ame",
        50,  75,  45,  80,  42,  85,  58,   true, false,false,false,  true,false, 0.78f,0.22f,0.12f);
    add(Inst::Rababah,"Wadi Hadramawt",    "Yemen",  "Am","Nahawand", "Profondeur du wadi, corde resineuse","profond",
        44,  68,  38,  65,  45,  82,  52,   true, false,false,false,  true,false, 0.72f,0.28f,0.15f);
    add(Inst::Rababah,"Tabuk Desert",      "Saudi",  "Bm","Rast",     "Solitude absolue, note tenue 8 sec","solitude",
        52,  85,  48,  70,  35,  80,  65,   true, false,false,false, false,false, 0.88f,0.20f,0.08f);
    add(Inst::Rababah,"Sahara Storyteller","Algeria","Am","Bayati",   "Griot saharien, histoire orale chantee","griot",
        38,  60,  60,  62,  38,  75,  50,   true, false, true,false,  true,false, 0.62f,0.35f,0.25f);

    // ══ MIZMAR (60-69) ═════════════════════════════════════════════════════════
    add(Inst::Mizmar,"Aswan Festival",    "Egypt",  "Gm","Hijaz Kar","Mizmar procession festival Assouan","procession",
        10,  45,  88,  45,  55,  70,  40,   true, false,false,false,  true,false, 0.45f,0.40f,0.20f);
    add(Inst::Mizmar,"Wedding Procession","Morocco","Em","Rast",     "Hautbois double mariage marocain","mariage",
        12,  50,  85,  48,  52,  68,  42,   true, false, true,false,  true,false, 0.50f,0.38f,0.30f);
    add(Inst::Mizmar,"Luxor Procession",  "Egypt",  "Cm","Bayati",   "Cortege pharaonique sur le Nil","nil",
        15,  55,  82,  55,  50,  72,  45,   true, false, true,false,  true,false, 0.55f,0.42f,0.28f);
    add(Inst::Mizmar,"Jeddah Seafront",   "Saudi",  "Fm","Hijaz",    "Mizmar port de Jeddah, mers chaudes","mer",
        12,  60,  78,  42,  48,  65,  48,   true, false, true,false,  true,false, 0.60f,0.38f,0.32f);
    add(Inst::Mizmar,"Giza Dawn",         "Egypt",  "Dm","Saba",     "Aube aux pyramides, premiers rayons","aube",
        20,  72,  65,  52,  38,  78,  55,   true, false,false,false,  true,false, 0.72f,0.28f,0.18f);
    add(Inst::Mizmar,"Khartoum Sufi",     "Sudan",  "Em","Kurd",     "Zikr soufi khartoum, cercle de priere","soufi",
        18,  78,  58,  60,  42,  82,  60,   true, false,false,false,  true,false, 0.78f,0.25f,0.15f);
    add(Inst::Mizmar,"Jerash Ruins",      "Jordan", "Cm","Nahawand", "Echo romain-arabe ruines de Jerash","ruines",
        15,  80,  72,  50,  45,  75,  65,   true,  true,false,false,  true,false, 0.78f,0.62f,0.15f);
    add(Inst::Mizmar,"Sfax Coastal",      "Tunisia","Dm","Rast",     "Brise cotiere, mizmar de pecheur","cote",
        12,  65,  80,  40,  40,  65,  50,   true, false, true,false,  true,false, 0.62f,0.35f,0.28f);
    add(Inst::Mizmar,"Aden Harbor",       "Yemen",  "Am","Hijaz",    "Port d'Aden, croisement d'oceans","port",
        15,  68,  75,  45,  48,  70,  52,   true, false, true,false,  true,false, 0.65f,0.38f,0.30f);
    add(Inst::Mizmar,"Omdurman Dust",     "Sudan",  "Bm","Bayati",   "Poussiere d'Omdurman, vent chaud sec","vent",
        18,  58,  70,  55,  52,  68,  45,   true, false,false,false,  true,false, 0.58f,0.32f,0.22f);

    // ─── Overrides ADSR per-preset ──────────────────────────────────────────
    // Indices: Violin 0-9, Oud 10-19, Ney 20-29, Qanun 30-39,
    //          Darbuka 40-49, Rababah 50-59, Mizmar 60-69

    // VIOLIN
    // [1] Desert Cinema Score — tremolo tendu, attaque rapide, sustain court
    factoryPresets[1].attack = 0.04f; factoryPresets[1].sustain = 0.55f; factoryPresets[1].release = 0.25f;
    // [3] Atlas Tears — harmonique, attaque lente, long release
    factoryPresets[3].attack = 0.20f; factoryPresets[3].sustain = 0.65f; factoryPresets[3].release = 0.80f;
    // [4] Beirut Lovers — legato romantique, vibrato lent
    factoryPresets[4].attack = 0.22f; factoryPresets[4].sustain = 0.80f; factoryPresets[4].release = 0.70f;
    // [7] Midnight Desert Scene — solo sombre, long release
    factoryPresets[7].attack = 0.30f; factoryPresets[7].sustain = 0.70f; factoryPresets[7].release = 1.20f;
    // [9] Aleppo Lament — plaintif, portamento, release très long
    factoryPresets[9].attack = 0.18f; factoryPresets[9].sustain = 0.72f; factoryPresets[9].release = 1.0f;

    // OUD
    // [10] Aladin Night Market — sec, percussif
    factoryPresets[10].attack = 0.003f; factoryPresets[10].decay = 0.30f; factoryPresets[10].sustain = 0.20f; factoryPresets[10].release = 0.50f;
    // [11] Thousand Nights — taqsim libre, soutenu
    factoryPresets[11].attack = 0.010f; factoryPresets[11].sustain = 0.45f; factoryPresets[11].release = 1.20f;
    // [14] Desert Caravan — tremolo, soutenu
    factoryPresets[14].attack = 0.008f; factoryPresets[14].decay = 0.50f; factoryPresets[14].sustain = 0.40f; factoryPresets[14].release = 1.0f;
    // [16] Palmyra Ghost — harmoniques, très long release
    factoryPresets[16].attack = 0.012f; factoryPresets[16].decay = 0.60f; factoryPresets[16].sustain = 0.30f; factoryPresets[16].release = 2.0f;

    // NEY
    // [20] Desert Wind Whisper — pianissimo, attaque douce
    factoryPresets[20].attack = 0.15f; factoryPresets[20].sustain = 0.90f; factoryPresets[20].release = 1.0f;
    // [22] Istanbul Bridge — classique ottoman, tenu
    factoryPresets[22].attack = 0.10f; factoryPresets[22].sustain = 0.85f; factoryPresets[22].release = 0.80f;
    // [26] Euphrates Breeze — brise, fluide
    factoryPresets[26].attack = 0.12f; factoryPresets[26].sustain = 0.82f; factoryPresets[26].release = 0.90f;
    // [28] Carthage Ruins — vent dans colonnes, très long release
    factoryPresets[28].attack = 0.20f; factoryPresets[28].sustain = 0.75f; factoryPresets[28].release = 1.50f;

    // QANUN
    // [30] Alexandria Elegance — rapide, ornements
    factoryPresets[30].attack = 0.003f; factoryPresets[30].decay = 0.40f; factoryPresets[30].sustain = 0.20f;
    // [36] Tarab Trance — émotionnel, soutenu
    factoryPresets[36].attack = 0.006f; factoryPresets[36].decay = 0.45f; factoryPresets[36].sustain = 0.35f; factoryPresets[36].release = 1.20f;
    // [39] Byblos Ancient — résonances anciennes, long release
    factoryPresets[39].attack = 0.005f; factoryPresets[39].decay = 0.60f; factoryPresets[39].sustain = 0.25f; factoryPresets[39].release = 1.50f;

    // DARBUKA
    // [43] Doumbek Drill — ultra précis, court
    factoryPresets[43].attack = 0.001f; factoryPresets[43].decay = 0.12f; factoryPresets[43].sustain = 0.02f; factoryPresets[43].release = 0.15f;
    // [47] Riyadh Trap — trap, short snappy
    factoryPresets[47].attack = 0.001f; factoryPresets[47].decay = 0.10f; factoryPresets[47].sustain = 0.01f; factoryPresets[47].release = 0.12f;
    // [48] Tripoli Trance — hypnotique, sustain plus long
    factoryPresets[48].attack = 0.002f; factoryPresets[48].decay = 0.25f; factoryPresets[48].sustain = 0.08f; factoryPresets[48].release = 0.30f;

    // RABABAH
    // [57] Tabuk Desert — note tenue 8 sec, release max
    factoryPresets[57].attack = 0.20f; factoryPresets[57].sustain = 0.80f; factoryPresets[57].release = 3.0f;
    // [56] Wadi Hadramawt — corde profonde, grave
    factoryPresets[56].attack = 0.18f; factoryPresets[56].sustain = 0.75f; factoryPresets[56].release = 0.90f;

    // MIZMAR
    // [60] Aswan Festival — procession énergique, attaque nette
    factoryPresets[60].attack = 0.03f; factoryPresets[60].sustain = 0.75f; factoryPresets[60].release = 0.30f;
    // [64] Giza Dawn — aube, attaque très douce
    factoryPresets[64].attack = 0.12f; factoryPresets[64].sustain = 0.78f; factoryPresets[64].release = 0.60f;
    // [65] Khartoum Sufi — zikr, sustain profond
    factoryPresets[65].attack = 0.08f; factoryPresets[65].sustain = 0.85f; factoryPresets[65].release = 0.80f;
    // ────────────────────────────────────────────────────────────────────────
}

void PresetManager::loadExtendedPresets()
{
    auto extended = PresetDataExtended::loadAll();
    for (auto& p : extended)
    {
        deriveADSR(p);
        factoryPresets.push_back(p);
    }
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
    obj->setProperty("attack",  preset.attack);
    obj->setProperty("decay",   preset.decay);
    obj->setProperty("sustain", preset.sustain);
    obj->setProperty("release", preset.release);

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
        preset.glide   = static_cast<float>(obj->getProperty("glide"));
        preset.space   = static_cast<float>(obj->getProperty("space"));
        preset.filter  = static_cast<float>(obj->getProperty("filter"));
        preset.orient  = static_cast<float>(obj->getProperty("orient"));
        preset.reso    = static_cast<float>(obj->getProperty("reso"));
        preset.depth   = static_cast<float>(obj->getProperty("depth"));
        preset.fxMix   = static_cast<float>(obj->getProperty("fxMix"));
        // ADSR — avec valeurs de fallback si preset ancien
        if (obj->hasProperty("attack"))
            preset.attack  = static_cast<float>(obj->getProperty("attack"));
        if (obj->hasProperty("decay"))
            preset.decay   = static_cast<float>(obj->getProperty("decay"));
        if (obj->hasProperty("sustain"))
            preset.sustain = static_cast<float>(obj->getProperty("sustain"));
        if (obj->hasProperty("release"))
            preset.release = static_cast<float>(obj->getProperty("release"));
    }
    return preset;
}

void PresetManager::setProUnlocked(bool unlocked)
{
    proUnlocked = unlocked;
}

bool PresetManager::isPresetAvailable(int index) const
{
    if (proUnlocked || index >= static_cast<int>(factoryPresets.size()))
        return true;

    // Demo mode: only first 3 instruments, 3 presets each
    const auto& preset = factoryPresets[index];
    int instIndex = static_cast<int>(preset.instrument);

    if (instIndex >= DEMO_NUM_INSTRUMENTS)
        return false;

    // Count presets for this instrument up to this point
    int presetCountForInst = 0;
    for (int i = 0; i <= index; ++i)
    {
        if (static_cast<int>(factoryPresets[i].instrument) == instIndex)
            presetCountForInst++;
    }

    return presetCountForInst <= DEMO_PRESETS_PER_INSTRUMENT;
}

int PresetManager::getMaxPresetsPerInstrument() const
{
    return proUnlocked ? OrientalConstants::PRESETS_PER_INSTRUMENT : DEMO_PRESETS_PER_INSTRUMENT;
}
