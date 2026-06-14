# GitHub Actions CI/CD Setup

Auto-build Oriental Instrument for iOS + Android on every commit.

---

## 🚀 Quick Start

### 1. Push to GitHub
```bash
git push origin feature/refonte-dsp-ui
```

### 2. Watch builds
- Go to: https://github.com/djbilboxbeats/DJBILBOX-VST/actions
- Select workflow (Android or iOS)
- Watch logs in real-time

### 3. Download APK/IPA
- Click workflow run
- Artifacts section → download APK or IPA

---

## 📦 Workflows Included

### `build-android.yml`
- Runs on: **Linux (Ubuntu)**
- Triggers: Every push + PR
- Builds: Debug + Release APK
- Output: `app-debug.apk`, `app-release.apk`
- Time: ~15 minutes

### `build-ios.yml`
- Runs on: **macOS**
- Triggers: Every push + PR
- Builds: Release + Demo IPA
- Output: `Oriental_Instrument_Release.ipa`, `Oriental_Instrument_Demo.ipa`
- Time: ~20 minutes

### `release.yml`
- Runs on: **Ubuntu + macOS**
- Triggers: Tag push (`git push origin v1.0.0`)
- Builds: Signed Release APK + IPA
- Creates: GitHub Release with downloads
- Requires: Signing credentials (see below)

---

## 🔐 Setup Signing (For Releases)

### Android Signing

1. **Create keystore** (first time only):
```bash
keytool -genkey -v -keystore oriental-release-key.jks \
  -keyalg RSA -keysize 2048 -validity 10000 \
  -alias oriental-key
```

2. **Encode to Base64**:
```bash
cat oriental-release-key.jks | base64 > keystore.txt
```

3. **Add to GitHub Secrets**:
   - Go to: Settings → Secrets and variables → Actions
   - Click: New repository secret
   - Name: `ANDROID_SIGNING_KEY`
   - Value: (paste keystore.txt contents)

4. **Add more secrets**:
   - `ANDROID_KEY_ALIAS`: `oriental-key` (from keytool)
   - `ANDROID_KEYSTORE_PASSWORD`: (your keystore password)
   - `ANDROID_KEY_PASSWORD`: (your key password)

### iOS Signing

1. **Export certificate** from Xcode:
   - Xcode → Preferences → Accounts → Manage Certificates
   - Right-click "Apple Distribution" → Export
   - Save as `certificates.p12`

2. **Encode to Base64**:
```bash
cat certificates.p12 | base64 > cert.txt
```

3. **Add to GitHub Secrets**:
   - Name: `APPLE_DEVELOPER_CERTIFICATE_P12_BASE64`
   - Value: (paste cert.txt)
   - Name: `APPLE_DEVELOPER_CERTIFICATE_PASSWORD`
   - Value: (your certificate password)

---

## 📱 Install on Devices

### Android (APK)

**Direct install** (needs USB debugging on phone):
```bash
# Download app-debug.apk from GitHub Actions
adb install app-debug.apk

# Or via file manager on phone
# Download APK → tap → Install
```

**Google Play** (for release):
1. Google Play Console → Upload APK
2. Set version code + release notes
3. Submit for review

### iOS (IPA)

**Via Xcode**:
1. Download `Oriental_Instrument_Release.ipa`
2. Xcode → Window → Devices and Simulators
3. Select device → drag .ipa file

**Via Apple Configurator** (easier):
1. Download app from App Store: "Apple Configurator 2"
2. Open → Add → select .ipa
3. Drag to device

**Via Testflight** (beta testing):
1. App Store Connect → TestFlight
2. Upload IPA
3. Add testers
4. Share link

---

## 🔄 Workflow Status

### Check Status
- **GitHub**: Actions tab → See real-time logs
- **Email**: Get notified if build fails
- **Badge**: Add to README:
  ```markdown
  ![Android Build](https://github.com/djbilboxbeats/DJBILBOX-VST/workflows/Build%20Android%20APK/badge.svg)
  ![iOS Build](https://github.com/djbilboxbeats/DJBILBOX-VST/workflows/Build%20iOS%20IPA/badge.svg)
  ```

### Debugging Failed Builds
1. Click failed workflow
2. Expand logs
3. Common issues:
   - JUCE not cloning: Check submodule settings
   - NDK/SDK not found: Update paths in workflow
   - Signing failed: Check secrets are set

---

## 📝 Files Created

```
.github/workflows/
├── build-android.yml    [Auto-build APK on push]
├── build-ios.yml        [Auto-build IPA on push]
└── release.yml          [Create release on tag]
```

---

## 🎯 Usage Examples

### Build on every commit (automatic)
```bash
git commit -m "feat: add new preset"
git push origin feature/refonte-dsp-ui
# → Android + iOS builds start automatically
# → Download APK/IPA from Actions in 15-20 min
```

### Create release
```bash
git tag v1.0.0
git push origin v1.0.0
# → Builds signed APK + IPA
# → Creates GitHub Release
# → Download from Releases page
```

### Demo vs Release
- **Every push**: Debug builds (full features, no signing needed)
- **Tag push**: Release builds (signed, ready for stores)

---

## ⚙️ Customization

### Change Android build variants
Edit `build-android.yml`:
```yaml
- name: Build Android (Release)
  run: |
    cd android
    ./gradlew assembleRelease  # Change to: assembleDebug, assembleDemo, etc.
```

### Change iOS deployment target
Edit `build-ios.yml`:
```yaml
-DCMAKE_OSX_DEPLOYMENT_TARGET=14.0  # Change to 13.0, 15.0, etc.
```

### Add notifications
Add to workflow (e.g., Slack):
```yaml
- name: Notify Slack
  if: failure()
  run: |
    curl -X POST ${{ secrets.SLACK_WEBHOOK }} \
      -d '{"text":"Build failed!"}'
```

---

## 📚 References

- [GitHub Actions Docs](https://docs.github.com/en/actions)
- [Android Gradle Plugin](https://developer.android.com/build)
- [Xcode Build System](https://developer.apple.com/xcode/)
- [Code Signing Guide](https://developer.apple.com/support/code-signing/)

---

## ✅ Setup Checklist

- [ ] Push code to GitHub repo
- [ ] Verify workflows in `.github/workflows/`
- [ ] Check Actions tab in GitHub
- [ ] (Optional) Add signing credentials for releases
- [ ] (Optional) Configure Slack notifications
- [ ] Test with tag push: `git tag v0.1.0 && git push origin v0.1.0`

**Result**: Automated APK + IPA on every commit. Download directly from GitHub Actions!
