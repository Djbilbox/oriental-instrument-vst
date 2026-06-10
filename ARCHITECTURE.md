# DJBILBOX VST — Architecture & décisions techniques

> Document vivant. Tenu à jour à chaque session de refonte.
> Dernière mise à jour : 2026-06-10 — refonte moteur sonore + UI haute fidélité.

---

## 1. Vue d'ensemble

Instrument virtuel oriental (VST3, JUCE/CMake). 7 instruments × 10 presets = 70 presets,
accordage maqam (quarts de ton), 8 effets, UI desert/glassmorphism.

```
PluginProcessor ──► OrientalSynthesiser ──► 16 × OrientalVoice ──► FXChain ──► out
       ▲                                         │
     APVTS ◄── PresetManager (70 presets) ───────┘
PluginEditor ──► BackgroundComponent (backdrop + glass + chrome)
            └──► KnobComponent / FXPanel / PresetBrowser / PianoComponent …
```

---

## 2. Moteur sonore (Mission 1)

### 2.1 Problème d'origine
Voix = 1 oscillateur wavetable additif (~5 harmoniques) + 1 filtre passe-bas IIR figé réglé
une seule fois au `noteOn`. Aucune saturation, aucun bruit, aucune modulation de filtre,
sortie mono dupliquée. Résultat : son « synthé pur », statique, sans grain ; presets
quasi identiques car seuls ADSR/cutoff variaient.

### 2.2 Nouvelle chaîne par voix (`OrientalVoice`)
```
3 osc unison détunés (pan stéréo équipuissant) + sub osc (-1 oct)
      → bruit / souffle filtré (one-pole) avec enveloppe propre
      → WAVESHAPER asymétrique   tanh(x + a) − tanh(a)   ← le "grain"
      → SVF résonant (juce::dsp::StateVariableTPTFilter)
            cutoff = base · 2^(envFiltre·amt + keyTrack + vélocité)
      → 2 FORMANTS parallèles (IIR bandpass) = corps/résonance de caisse
      → ampli ADSR → soft-clip de cohésion (tanh) → stéréo
```

| Décision | Raison |
|---|---|
| **Waveshaper asymétrique** (biais DC compensé) | Génère harmoniques **paires** → chaleur organique type tube/Taqsim, pas une simple distorsion symétrique. |
| **SVF TPT** au lieu d'`IIR::makeLowPass` statique | Filtre **vivant** : enveloppe dédiée + key-tracking + vélocité ; résonance jusqu'à auto-oscillation (Q 0.5→8). |
| **2 bandpass parallèles** ajoutés au signal | Modélisent les pics formantiques de la caisse (oud, qanun) — coefficients recalculés au `noteOn` uniquement. |
| **Unison 3 voix + sub + bruit** | Épaisseur, largeur stéréo, transitoires/souffle réalistes (pluck chiff, breath, bow scrape). |
| **Caractère porté par le profil d'instrument** | Chaque instrument a drive/asym/bruit/formants/enveloppe-filtre distincts → presets réellement différenciés. |

### 2.3 Re-mapping des macros (APVTS **inchangé** → aucune casse de compat/preset)
- **DEPTH** → intensité de saturation/grain : `drive · (0.6 + depth·1.4)`
- **ORIENT** → vibrato + largeur de détune unison + mouvement
- **FILTER / RESO** → cutoff/résonance du SVF (désormais modulés par enveloppe)
- **Vélocité** → cutoff **et** drive (dynamique expressive)

### 2.4 Mapping des 70 presets
`PresetData` reste inchangé (glide/space/filter/orient/reso/depth/fxMix + ADSR + FX).
La **saturation et la dynamique** ne sont pas des champs de preset : elles émergent du
profil d'instrument (drive/asym/formants) modulé par DEPTH et la vélocité.
`PresetManager::applyInstrumentADSR()` lit `profile.attack/decay/sustain/release`
(champs conservés) → compilable et opérationnel.

### 2.5 Coût temps réel
SVF : `tan()` par échantillon × 2 canaux × ≤16 voix ≈ négligeable sur CPU desktop.
Formants : coefficients calculés au `noteOn` seulement. Aucune allocation dans le thread audio.

### 2.6 Pistes futures (non implémentées)
Oscillateur band-limited (mip wavetable) anti-aliasing ; oversampling 2× autour du waveshaper.

---

## 3. UI haute fidélité (Mission 2)

### 3.1 Bug structurel corrigé
`PluginEditor::paint()` dessinait header + panneaux glass + bordures **derrière** le
`BackgroundComponent` (un enfant peint par-dessus le parent en JUCE) → **invisibles**.
D'où l'aspect « plat » : on ne voyait que le désert + les contrôles flottants.

**Décision** : toute la chrome (header, panneaux, labels, séparateurs) et le relief glass
sont déplacés dans `BackgroundComponent`, qui se situe derrière les contrôles mais
au-dessus du désert. `PluginEditor::paint()` est désormais vide.

### 3.2 BackgroundComponent
- **Décor complet porté du prototype HTML** : toutes les figures SVG sont
  désormais redessinées en `juce::Path` — ciel, étoiles + lune, soleil, 3 pyramides,
  **Sphinx**, dunes, **4 musiciens Gnawa** (guembri, qraqeb, danseur), **dromadaire**
  (selle touareg), **2 aigles** en vol, **palmeraie**, brume de chaleur, vignettage.
  Les coordonnées SVG (repère 980×640) sont mappées par `sx=w/980, sy=h/640`.
- **Cache `juce::Image`** : la scène entière est rendue **une fois** dans `cachedDesert`
  (régénérée sur changement de taille) — les figures ne coûtent rien par frame.
- **Halo animé** : `juce::Timer` à 30 fps fait respirer l'intensité du soleil
  (`0.82 + 0.18·sin(phase)`), dessiné en direct par-dessus le cache.
- **Glassmorphism réel** : `cachedFrosted` = désert + soleil **floutés** une fois via
  `BlurUtils::frosted()`. Les panneaux glass affichent cette image floutée détourée
  (clip) + teinte chaude translucide (~65 %, plus transparente qu'avant) + sheen spéculaire
  + **bordure dégradée or** (clair en haut → sombre en bas).

### 3.3 BlurUtils (`Source/GUI/BlurUtils.h`, header-only)
Box blur séparable horizontal+vertical, 3 passes ≈ gaussienne, fenêtre glissante
(coût indépendant du rayon). Usage **one-shot** (resize), jamais par frame.
Choix d'une implémentation maison : JUCE n'embarque pas de stack-blur natif performant.

### 3.4 Knobs (`OrientalLookAndFeel::drawRotarySlider`)
3 couches de relief : **(1)** ombre portée externe ; **(2)** corps métal brossé
(dégradé radial + striations radiales fines détourées au disque) + biseau ;
**(3)** glint spéculaire haut-gauche. Arc de valeur avec **glow en 3 strates**
translucides (simule un flou — moins coûteux qu'un blur d'image par frame).

### 3.5 Typographie (`Source/GUI/Typography.h`, header-only)
Hiérarchie unique et stricte : `title` (Cinzel 22 bold) / `subtitle` / `sectionLabel` /
`knobLabel` / `value` / `tab`. Helper `tracked()` = capitales + fines espaces (JUCE n'a
pas de letter-spacing).
**TODO BinaryData** : déposer `Cinzel.ttf` / `Inter.ttf` dans `Resources/Fonts`,
ajouter `juce_add_binary_data()` au CMake, puis remplacer les lookups par nom par
`Typeface::createSystemTypefaceFor(BinaryData::…)`. Tant que les .ttf sont absents,
fallback gracieux vers la police système.

---

## 4. Fichiers touchés cette session
**DSP** : `InstrumentProfiles.{h,cpp}`, `OrientalVoice.{h,cpp}`.
**UI** : `BackgroundComponent.{h,cpp}`, `OrientalLookAndFeel.cpp`, `KnobComponent.cpp`,
`PluginEditor.cpp`, + nouveaux `GUI/BlurUtils.h`, `GUI/Typography.h`.
Headers-only → aucune modif CMake nécessaire.

## 5. Validation
- Compilation : via CI (`.github/workflows/build.yml`, récupère JUCE) ou
  `Builds/VisualStudio2022`. JUCE n'est pas présent en local → pas de build local.
- Audio : charger le VST3, comparer 2–3 presets par instrument (oud claquant vs ney
  soufflé vs mizmar agressif) ; vérifier l'ouverture de filtre à l'attaque et le grain
  à DEPTH élevé.
- Visuel : profondeur des panneaux (désert flouté visible), relief des knobs, hiérarchie typo.
