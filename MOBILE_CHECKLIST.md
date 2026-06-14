# Mobile App Release Checklist

## Code & Build
- [x] Shared DSP foundation (C++ portable)
- [x] Audio engine wrapper (MobileAudioContext)
- [x] Mobile UI abstraction (MobileUIController)
- [x] IAP abstractions (iOSIAPManager, AndroidIAPManager)
- [x] Freemium preset filtering (PresetManager)
- [x] Demo watermark indicator (FreemiumIndicator)
- [x] iOS target in CMakeLists
- [x] Android target in CMakeLists
- [ ] Resolve compilation warnings (TBD by agents)
- [ ] Full touch UI implementation (knobs, keyboard, presets for mobile)
- [ ] Portrait/landscape layout refinement
- [ ] Dark mode support (iOS 13+)
- [ ] Hardware keyboard support (iPad, Android)

## iOS Specifics
- [ ] Apple Developer account (membership required)
- [ ] App Store Connect app record created
- [ ] Bundle ID registered (com.djbilbox.orientalinstrument.ios)
- [ ] Provisioning profile configured
- [ ] Development/Production certificates installed
- [ ] StoreKit 2 integration (IAP full implementation)
- [ ] TestFlight beta setup
- [ ] App review submission checklist

## Android Specifics
- [ ] Google Play Developer account (one-time fee)
- [ ] App listing created in Google Play Console
- [ ] Package name registered (com.djbilbox.orientalinstrument.android)
- [ ] Release signing keystore created
- [ ] Google Play Billing Library integration (full implementation)
- [ ] Internal testing track setup
- [ ] App review submission checklist

## UI/UX
- [ ] Touch knob controls (drag-to-adjust)
- [ ] On-screen MIDI keyboard (velocity-sensitive if possible)
- [ ] Preset browser (search, tags, grid/list view)
- [ ] Instrument selector (tabs or menu)
- [ ] FX panel (enable/disable, parameter knobs)
- [ ] Waveform display (real-time visualization)
- [ ] Header (preset name, transport buttons, settings)
- [ ] Settings screen (MIDI input, audio output, reset, about)

## Performance & Testing
- [ ] CPU usage profiling (target: <30% on iPhone 12)
- [ ] Memory profiling (no leaks, stable over 1+ hour)
- [ ] Audio latency measurement (target: <100ms)
- [ ] Battery drain test (1 hour play time)
- [ ] Thermal testing (no device overheating)
- [ ] Screen rotation handling
- [ ] Background audio resume (interruption handling)
- [ ] MIDI controller compatibility test

## Audio Quality
- [ ] DSP engine ported & functional
- [ ] Maqam tuning verified
- [ ] Instrument presets playable
- [ ] Effects chain functional
- [ ] No audio glitches/pops
- [ ] Mono/legato modes work
- [ ] Sample playback (if enabled)

## IAP & Monetization
- [ ] "oriental_pro" product created on both platforms
- [ ] Price tier confirmed ($4.99 USD)
- [ ] Freemium watermark visible in demo
- [ ] Purchase flow tested (end-to-end)
- [ ] Restore purchases flow tested
- [ ] Receipt validation implemented
- [ ] Demo preset limit enforced

## Security & Privacy
- [ ] Privacy policy finalised + hosted
- [ ] Audio NOT recorded/sent to servers
- [ ] Presets: local storage only (unless cloud opt-in)
- [ ] No analytics/tracking (unless explicitly added)
- [ ] Permissions requested (audio input only)
- [ ] Terms of Service drafted (if required)
- [ ] GDPR compliance review

## Documentation
- [ ] User manual (in-app help or web)
- [ ] Keyboard shortcuts documented (iPad)
- [ ] MIDI mapping documented
- [ ] Maqam tuning explanation
- [ ] Preset naming conventions
- [ ] Credits/license (JUCE, fonts, etc.)

## Submission
- [ ] App Icon (1024×1024 for iOS, multiple sizes for Android)
- [ ] Screenshot set (at least 2-5 per platform)
- [ ] App description finalized
- [ ] Keywords/tags curated
- [ ] Category selected (Music)
- [ ] Content rating filled (4+ / Everyone)
- [ ] Support email configured (djbilbox@gmail.com)
- [ ] Website URL (https://djbilboxbeats.github.io/)

## Post-Launch
- [ ] Monitor crash reports (Xcode Organizer, Play Console)
- [ ] Respond to user reviews
- [ ] Plan v1.1 updates (UI refinements, new presets)
- [ ] Gather analytics on feature usage
- [ ] Bug fix response plan
- [ ] Community feedback channels (Twitter, email)

---

## Timeline Estimate

**Phase 1 (Weeks 1-2)**: Code foundation + build setup ✅ (In progress)
**Phase 2 (Weeks 3-4)**: Touch UI implementation + IAP full integration
**Phase 3 (Week 5)**: Testing + performance optimization
**Phase 4 (Week 6)**: App store submission preparation
**Phase 5 (Week 7+)**: Review feedback + launch

**Target Launch**: End of July 2026
