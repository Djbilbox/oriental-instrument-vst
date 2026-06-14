# Installer Oriental Instrument sur tes deux téléphones

**Goal**: APK (Android) + IPA (iOS) téléchargés et installés sur tes appareils.

---

## 🚀 ÉTAPE 1: Build automatique (GitHub Actions)

### Prérequis
- Compte GitHub (gratuit)
- Repo poussé à GitHub

### Marche à suivre

1. **Pousse le code**:
```bash
cd "C:\Users\djbil\Desktop\ORIENTAL VST\DJBILBOX VST"
git remote add origin https://github.com/TonUsername/DJBILBOX-VST.git
git push -u origin feature/refonte-dsp-ui
```

2. **Regarde les builds**:
   - Va à: https://github.com/TonUsername/DJBILBOX-VST/actions
   - Clique sur "Build Android APK" ou "Build iOS IPA"
   - Attends ~15-20 min (construction en cours)

3. **Télécharge APK/IPA**:
   - Une fois terminé, clique sur le workflow
   - Bas de la page → "Artifacts"
   - Télécharge:
     - `app-debug.apk` (Android)
     - `Oriental_Instrument_Release.ipa` (iOS)

---

## 📱 ÉTAPE 2: Installer sur Android

### Option A: USB Direct (Rapide)

**Besoin**:
- Câble USB
- Android Debug Bridge (adb)

**Installation adb** (si pas déjà):
```bash
# Windows: Télécharge https://developer.android.com/tools/releases/platform-tools
# Ajoute au PATH

adb version  # Vérifier
```

**Marche**:
1. Connecte ton téléphone Android par USB
2. Active "USB Debugging": Settings → Developer Options → USB Debugging
3. Sur PC:
```bash
# Télécharge app-debug.apk depuis GitHub Actions
cd "C:\Users\djbil\Downloads"

adb install app-debug.apk
# Attends ~30 sec

adb shell am start -n com.djbilbox.orientalinstrument.android/.MainActivity
```

4. App se lance sur ton téléphone!

### Option B: QR Code / Lien (Sans câble)

1. Sur GitHub Actions, clique "app-debug.apk"
2. Copie le lien de téléchargement
3. Envoie le lien (WhatsApp, email, etc.)
4. Sur ton Android:
   - Ouvre le lien
   - "Télécharger" → "Installer"
   - Ouvre l'app depuis le tiroir d'apps

---

## 🍎 ÉTAPE 3: Installer sur iOS

### Option A: via Mac (Si tu as un Mac)

**Besoin**:
- Mac avec Xcode
- Câble USB-C ou Lightning

**Marche**:
1. Télécharge `Oriental_Instrument_Release.ipa` de GitHub Actions
2. Sur Mac:
```bash
# Connecte iPhone
open /Applications/Xcode.app

# Window → Devices and Simulators
# Sélectionne ton iPhone
# Drag & drop .ipa file
```

3. App s'installe!

### Option B: Apple Configurator (Plus simple)

1. Sur Mac, télécharge "Apple Configurator 2" (gratuit sur App Store)
2. Ouvre → "Add" → Sélectionne .ipa
3. Connecte iPhone
4. Drag .ipa sur iPhone dans Configurator
5. Approuve sur iPhone quand demandé

### Option C: TestFlight (Sans câble)

1. Demande à moi (ou crée un Apple Developer account)
2. Je te fais un lien TestFlight
3. Clique sur iOS → Ouvre lien
4. "Accepter" → App s'installe via App Store

---

## ✅ Vérification

### Android
```bash
adb logcat | grep Oriental  # Regarde les logs
adb shell pm list packages | grep djbilbox  # Vérifie install
```

### iOS
1. Settings → General → Storage → Vérifie "Oriental Instrument"
2. Appuie sur app → "Offload" = instalée

---

## 🎵 Test de l'app

1. **Ouvre**:
   - Android: Tiroir d'apps → Oriental Instrument
   - iOS: Home screen → Oriental Instrument

2. **Joue du clavier**:
   - Écran tactile: Appuie sur les touches
   - Glisse verticalement: Pitch-bend (montée/descente)

3. **Essaie les presets**:
   - Menu présets → Sélectionne un preset
   - Son change

4. **Teste IAP** (achat intégré):
   - "Demo" watermark visible
   - Clique pour unlock (test mode)

---

## 📋 Troubleshooting

### Android

**"app-debug.apk not found"**
→ Attends que GitHub Actions finisse (15 min)

**"adb: command not found"**
→ Installe Android Platform-Tools, ajoute au PATH

**"Device not found"**
→ Connecte USB, active Developer Mode + USB Debugging

### iOS

**"IPA not compatible"**
→ iPhone doit être iOS 14.0+ (Settings → General → About)

**"Certificate not trusted"**
→ Settings → General → VPN & Device Management → Approuve le cert

**"App won't launch"**
→ Offload → Réinstalle depuis TestFlight ou Apple Configurator

---

## 🔄 Updates

Une fois installé, pour updates:

**Android**:
```bash
# Télécharge nouvel app-debug.apk
adb uninstall com.djbilbox.orientalinstrument.android
adb install app-debug.apk
```

**iOS**:
- Offload + réinstalle
- Ou mets à jour via TestFlight automatiquement

---

## 📞 Questions?

**Pour builds / GitHub Actions**: Voir `CI_CD_SETUP.md`  
**Pour développement**: Voir `ORIENTAL_MOBILE_SETUP.md`  
**Pour architecture**: Voir `PROJECT_SUMMARY.md`

---

## 🎯 Résumé

| Plateforme | Fichier | Install | Temps |
|-----------|---------|---------|-------|
| Android | app-debug.apk | `adb install` | 2 min |
| iOS | .ipa | Xcode / Configurator | 5 min |

**Total**: ~20 min build + 5 min install = **Jouable en 25 min!**

Enjoy Oriental Instrument sur tes phones! 🎵
