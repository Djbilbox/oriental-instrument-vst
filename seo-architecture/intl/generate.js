#!/usr/bin/env node
/* DJBILBOX BEATS — programmatic spoke generator (TRUTH-ONLY build).
   Reads 00-facts + 01-architecture + 02-injection-variables, emits raw .html
   per spoke + hub index pages + sitemap + robots + IndexNow key.
   NO invented ratings, prices, experience years, or CPU benchmarks.
   Run: node generate.js [outDir]                                              */

const fs = require('fs');
const path = require('path');

const HERE = __dirname;
const facts = JSON.parse(fs.readFileSync(path.join(HERE, '00-djbilbox-facts.json'), 'utf8'));
const arch = JSON.parse(fs.readFileSync(path.join(HERE, '01-architecture-international.json'), 'utf8'));
const vars = JSON.parse(fs.readFileSync(path.join(HERE, '02-dynamic-injection-variables.json'), 'utf8'));
const OUT = process.argv[2] || path.join(HERE, 'pages');
const DOMAIN = arch.site.domain;
const OI = facts.whatIsTrue.vstDevelopment.realProduct; // Oriental Instrument facts

fs.mkdirSync(OUT, { recursive: true });

const hash = s => { let h = 0; for (const c of s) h = (h * 31 + c.charCodeAt(0)) | 0; return Math.abs(h); };
const pick = (list, slug) => list[hash(slug) % list.length];
const esc = s => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');

const CSS = fs.readFileSync(path.join(HERE, '03-template-spoke.html'), 'utf8').match(/<style>[\s\S]*?<\/style>/)[0];

const head = p => `<!DOCTYPE html><html lang="${p.lang}"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>${esc(p.title)}</title>
<meta name="description" content="${esc(p.meta)}">
<link rel="canonical" href="${p.url}">
<meta name="robots" content="index,follow,max-image-preview:large">
${p.hreflang || ''}
<meta property="og:type" content="website"><meta property="og:title" content="${esc(p.title)}">
<meta property="og:description" content="${esc(p.meta)}"><meta property="og:url" content="${p.url}">
<meta name="twitter:card" content="summary_large_image">
${CSS}</head><body>
<header class="top"><div class="bar">
<a class="brand" href="/">DJBILBOX BEATS</a>
<a href="/vst-plugins/">VST Plugins</a><a href="/beats/">Beats</a><a href="/web-dev-music/">Web Dev</a>
<a href="#lead" class="hero-cta" style="padding:.45rem 1rem">${p.lang === 'fr' ? 'Devis' : 'Get a Quote'}</a></div></header>
<nav class="crumb" aria-label="Breadcrumb">${p.crumb}</nav>`;

const tocHtml = toc => toc.map(t => `<a href="#${t.id}">${esc(t.label)}</a>`).join('');
const faqHtml = faq => faq.map((f, i) => `<details${i === 0 ? ' open' : ''}><summary>${esc(f.q)}</summary><p>${esc(f.a)}</p></details>`).join('');

/* ---- honest schema: NO aggregateRating, NO invented price. Offers only when real. ---- */
const schema = p => {
  const graph = [
    { '@type': 'BreadcrumbList', itemListElement: p.crumbItems.map((c, i) => ({ '@type': 'ListItem', position: i + 1, name: c.name, item: c.item })) },
    { '@type': 'MusicGroup', '@id': DOMAIN + '/#musicgroup', name: facts.brand, url: DOMAIN + '/', genre: facts.whatIsTrue.beatmaking.styles, foundingLocation: { '@type': 'Place', name: 'Paris, France' }, areaServed: ['US', 'GB', 'FR', 'DE', 'AE', 'Worldwide'], email: facts.email },
    { '@type': 'ProfessionalService', '@id': DOMAIN + '/#service', name: facts.brand + ' — Audio plugin, beats & music web engineering', url: p.url, serviceType: p.serviceType, provider: { '@id': DOMAIN + '/#musicgroup' }, areaServed: ['Paris', 'Toulouse', 'London', 'Los Angeles', 'Atlanta', 'Berlin', 'Dubai', 'Worldwide'], knowsAbout: ['C++', 'JUCE', 'DSP', 'VST3', 'AU', 'beatmaking', 'music web development'] }
  ];
  // real product schema for VST context only
  if (p.includeProduct) {
    graph.push({
      '@type': 'SoftwareApplication', '@id': DOMAIN + '/#oriental-instrument', name: OI.name,
      description: `${OI.type}. ${OI.instrumentCount} instruments (${OI.instruments.join(', ')}), ${OI.presets} presets, ${OI.effects} effects, ${OI.microtonal}. ${OI.status}`,
      applicationCategory: 'MultimediaApplication', applicationSubCategory: 'Audio Plugin (VST3 / AU)',
      operatingSystem: 'Windows, macOS, iOS, Android', author: { '@id': DOMAIN + '/#musicgroup' },
      offers: { '@type': 'Offer', priceCurrency: 'USD', price: '4.99', description: 'Oriental Instrument mobile — Pro in-app unlock', availability: 'https://schema.org/PreOrder' }
    });
  }
  graph.push({ '@type': 'FAQPage', mainEntity: p.faq.map(f => ({ '@type': 'Question', name: f.q, acceptedAnswer: { '@type': 'Answer', text: f.a } })) });
  return JSON.stringify({ '@context': 'https://schema.org', '@graph': graph }, null, 0);
};

const render = p => `${head(p)}
<div class="wrap">
<aside class="toc" aria-label="On this page"><strong>${p.lang === 'fr' ? 'Sur cette page' : 'On this page'}</strong>${tocHtml(p.toc)}</aside>
<main>
<h1>${esc(p.h1)}</h1>
<p class="intro">${p.intro}</p>
<a href="#lead" class="hero-cta">${esc(p.ctaPrimary)}</a>
<a href="${p.ctaSecHref}" class="hero-cta ghost">${esc(p.ctaSec)}</a>
<section id="store" class="store" aria-label="${esc(p.storeHeading)}">
<h2 style="margin-top:.2rem">${esc(p.storeHeading)}</h2>
<div class="player" style="height:120px;display:flex;align-items:center;gap:1rem;padding:0 1.2rem;border:1px solid var(--line);border-radius:8px;background:linear-gradient(90deg,#14141c,#1d1530)">
<div style="width:48px;height:48px;border-radius:50%;background:var(--acc);display:flex;align-items:center;justify-content:center">▶</div>
<div style="flex:1;height:40px;background:repeating-linear-gradient(90deg,var(--acc2) 0 2px,transparent 2px 6px);opacity:.5;border-radius:4px"></div></div>
<p style="color:var(--mut);font-size:.85rem;margin-top:.6rem">${esc(p.storeNote)}</p></section>
${p.body}
<form id="lead" class="lead" action="${DOMAIN}/api/lead" method="post">
<input name="email" type="email" required placeholder="you@studio.com">
<input name="project" placeholder="${esc(p.leadPlaceholder)}">
<button class="hero-cta" type="submit">${esc(p.ctaPrimary)}</button></form>
<p style="text-align:center;color:var(--mut)">${p.lang === 'fr' ? 'Décris ton projet, je reviens avec périmètre et tarif.' : "Tell me what you need and I'll come back with scope and pricing."} <a href="mailto:${facts.email}">${facts.email}</a></p>
<h2 id="faq">FAQ</h2>${faqHtml(p.faq)}
<p style="margin-top:2rem">${p.lang === 'fr' ? 'À voir aussi' : 'Related'}: ${p.related}</p>
</main></div>
<footer>© <span id="y"></span> ${facts.brand} — Paris · Toulouse · Worldwide · <a href="/vst-plugins/">VST</a> · <a href="/beats/">Beats</a> · <a href="/web-dev-music/">Web</a></footer>
<script>document.getElementById('y').textContent=new Date().getFullYear();var ls=[].slice.call(document.querySelectorAll('aside.toc a')),hs=ls.map(function(a){return document.querySelector(a.getAttribute('href'))});addEventListener('scroll',function(){var y=scrollY+90,i=hs.length;while(--i>=0){if(hs[i]&&hs[i].offsetTop<=y){ls.forEach(function(a){a.classList.remove('on')});ls[i].classList.add('on');break}}},{passive:true});</script>
<script type="application/ld+json">${schema(p)}</script>
</body></html>`;

/* ---------- BEATS spokes (Genre x City) — styles are real, no invented sales numbers ---------- */
function beatPage(s) {
  const slug = s.path.split('/').filter(Boolean).pop();
  const g = vars.axes.genre[s.genre] || Object.values(vars.axes.genre)[0];
  const c = vars.axes.city[s.city] || {};
  const title = pick(vars.axes.titlePatterns.beats, slug).replace('{Genre}', s.genre).replace(/\{City\}/g, s.city);
  return {
    file: slug + '.html', lang: 'en', url: DOMAIN + s.path, serviceType: `${s.genre} beat production`,
    title,
    meta: `${s.genre} beats produced for the ${s.city} sound — ${s.scene}. Typically ${g.bpmRange} BPM, ${g.keyScale}. Lease, exclusive or custom by DJBILBOX BEATS.`,
    h1: `${s.genre} Beats — ${s.city}`,
    intro: `<strong>${esc(s.genre)} beats</strong> made for the ${esc(s.city)} sound: ${esc(s.scene)}. Typically ${esc(g.bpmRange)} BPM, ${esc(g.keyScale)} tonality, with ${esc(g.signature.join(', '))}. Available to lease, take exclusive, or commission fully custom.`,
    ctaPrimary: 'Browse on Beatstars', ctaSec: 'Commission a Custom Beat', ctaSecHref: '#lead',
    storeHeading: `${s.genre} — ${s.city}`,
    storeNote: 'Beatstars player embeds here (live beat store).',
    leadPlaceholder: `Which ${s.genre} vibe do you need?`,
    crumb: `<a href="/">Home</a> › <a href="/beats/">Beats</a> › <span>${esc(s.genre)} ${esc(s.city)}</span>`,
    crumbItems: [{ name: 'Home', item: DOMAIN + '/' }, { name: 'Beats', item: DOMAIN + '/beats/' }, { name: `${s.genre} ${s.city}`, item: DOMAIN + s.path }],
    toc: [{ id: 'store', label: 'Beat store' }, { id: 'scene', label: `${s.city} scene` }, { id: 'sound', label: 'The sound' }, { id: 'who', label: "Who it's for" }, { id: 'faq', label: 'FAQ' }, { id: 'lead', label: 'Get a beat' }],
    body: `
<h2 id="scene">The ${esc(s.city)} ${esc(s.genre)} sound</h2>
<p>${esc(s.city)} is associated with ${esc(c.localScene || s.scene)}. Charts and stations to know: ${esc(c.chartReference || 'local streaming charts')}.${c.studioHeritage ? ' Studio heritage: ' + esc(c.studioHeritage.join(', ')) + '.' : ''} Beats here aim at that pocket, not a generic preset.</p>
<h2 id="sound">The sound — ${esc(g.bpmRange)} BPM</h2>
<p>Built around ${esc(g.signature.join(', '))} in a ${esc(g.keyScale)} tonality, mixed for translation across club, phone and car. Stems available on request.</p>
<div class="grid">
<div class="card"><h3>Lease</h3><p>MP3 + WAV, non-exclusive license. Quick for demos and singles. Pricing on Beatstars.</p></div>
<div class="card"><h3>Exclusive</h3><p>Full rights transfer + trackouts; beat pulled from store. For releases.</p></div>
<div class="card"><h3>Custom</h3><p>Commissioned to your reference, BPM and key, for ${esc(g.audience)}.</p></div>
</div>
<h2 id="who">Who it's for</h2>
<p>${esc(g.audience)} after an authentic ${esc(s.city)} ${esc(s.genre)} record. Need a custom oriental sound? See <a href="/vst-plugins/juce-cpp-developer/">Oriental Instrument &amp; custom VST work</a>. Need a release site? <a href="/web-dev-music/">Music web dev</a>.</p>`,
    related: `<a href="/beats/">All beats</a> · <a href="/vst-plugins/juce-cpp-developer/">Custom VST tools</a> · <a href="/web-dev-music/artist-landing-page-creator/">Artist landing pages</a>`,
    faq: [
      { q: `What BPM and key are your ${s.genre} beats?`, a: `Usually ${g.bpmRange} BPM in ${g.keyScale}. Custom commissions match any BPM/key you specify.` },
      { q: 'Lease vs exclusive?', a: 'Lease is a non-exclusive license (MP3/WAV). Exclusive transfers full rights, includes stems and removes the beat from the store. Lease prices are listed on Beatstars.' },
      { q: 'Do I get stems / trackouts?', a: 'Included with exclusive and custom; available as an add-on on leases.' },
      { q: `Can you match a specific ${s.city} reference?`, a: 'Yes — send the reference on a custom commission and I match the pocket, instrumentation and mix character.' }
    ]
  };
}

/* ---------- VST + WEB service spokes — grounded in Oriental Instrument, no invented metrics ---------- */
function servicePage(s, hub) {
  const slug = s.slug || s.path.split('/').filter(Boolean).pop();
  const isVst = hub === 'vst-plugins';
  const lang = s.locale === 'fr' ? 'fr' : 'en';
  const title = pick(isVst ? vars.axes.titlePatterns.vst : vars.axes.titlePatterns.web, slug).replace(/\{Service\}/g, s.title);
  return {
    file: slug + '.html', lang, url: DOMAIN + s.path, includeProduct: isVst, serviceType: s.title,
    title,
    meta: isVst
      ? `${s.title} in C++ / JUCE — VST3 & AU for Windows & macOS. Builder of Oriental Instrument (oriental sampler, Maqam tuning). Request a quote.`
      : `${s.title} — ultra-light raw HTML5/CSS3/JS, fast and SEO-ready, for musicians, studios and labels. ${(s.keywords || []).slice(0, 2).join(', ')}.`,
    h1: s.title,
    intro: isVst
      ? `<strong>${esc(s.title)}</strong> built in C++ with the JUCE framework — VST3 and AU for Windows and macOS. The same stack runs my own product, <strong>${esc(OI.name)}</strong> (an oriental sampler with ${esc(OI.microtonal)}), so this isn't theory. Real-time-safe audio thread, multi-sampling, custom DSP tuned to your material.`
      : `<strong>${esc(s.title)}</strong> — raw HTML5/CSS3/JS, no framework bloat, built for speed and SEO (this very SEO cocoon is an example of the approach). For musicians, studios and labels who want an owned, fast site.`,
    ctaPrimary: isVst ? 'Request a Plugin Quote' : 'Request a Site Quote', ctaSec: 'See the JUCE proof of work', ctaSecHref: '/vst-plugins/juce-cpp-developer/',
    storeHeading: isVst ? 'Oriental Instrument — demo' : 'Live demo embed',
    storeNote: isVst ? 'Oriental Instrument audio demo embeds here (product in development, beta to come).' : 'Live site preview / Beatstars embed here.',
    leadPlaceholder: isVst ? 'What plugin do you need?' : 'What site do you need?',
    crumb: `<a href="/">Home</a> › <a href="${arch.hubs[hub].path}">${esc(arch.hubs[hub].h1)}</a> › <span>${esc(s.title)}</span>`,
    crumbItems: [{ name: 'Home', item: DOMAIN + '/' }, { name: arch.hubs[hub].h1, item: DOMAIN + arch.hubs[hub].path }, { name: s.title, item: DOMAIN + s.path }],
    toc: [{ id: 'store', label: isVst ? 'Demo' : 'Demo' }, { id: 'what', label: 'What you get' }, { id: 'tech', label: 'Tech' }, { id: 'process', label: 'Process' }, { id: 'faq', label: 'FAQ' }, { id: 'lead', label: 'Get a quote' }],
    body: `
<h2 id="what">What you get</h2>
<div class="grid">
${(isVst
        ? ['Custom DSP tuned to your references, in C++/JUCE', 'VST3 + AU for Windows & macOS (the Oriental Instrument stack); iOS/Android possible', 'Real-time-safe audio thread — no allocations or locks', 'Source handover on custom engagements — you own the IP']
        : ['Raw HTML5/CSS3/JS — no CMS bloat', 'Fast, accessible, SEO-structured pages you own', 'Beatstars / Spotify / smartlink integration', 'Programmatic SEO architecture (semantic silos + JSON-LD)'])
      .map(x => `<div class="card"><p>${esc(x)}</p></div>`).join('')}
</div>
<h2 id="tech">The tech</h2>
<p>${isVst
        ? 'Stack: ' + esc(vars.axes.vstTechnical.dspConcepts.slice(0, 6).join(', ')) + '. Formats shipped: ' + esc(vars.axes.vstTechnical.marketSpec.shipped) + '. ' + esc(vars.axes.vstTechnical.marketSpec.onRequest) + '. No fixed CPU benchmark is published — performance is measured per project.'
        : 'Stack: ' + esc(vars.axes.webDevTechnical.stack.join(', ')) + '. Integrations: ' + esc(vars.axes.webDevTechnical.integrations.join(', ')) + '.'}</p>
<h2 id="process">Process</h2>
<p>${lang === 'fr' ? 'Appel de cadrage → devis à périmètre fixe → prototype que tu valides → build + QA → livraison avec sources.' : 'Scope call → fixed-scope quote → prototype you A/B → build + QA → handover with source.'}</p>`,
    related: isVst
      ? `<a href="/vst-plugins/juce-cpp-developer/">JUCE C++ developer</a> · <a href="/vst-plugins/dsp-optimization-service/">DSP optimization</a> · <a href="/beats/">Beats</a>`
      : `<a href="/web-dev-music/music-label-web-developer/">Label web dev</a> · <a href="/vst-plugins/">VST plugins</a> · <a href="/beats/">Beats</a>`,
    faq: isVst
      ? [{ q: 'Which formats do you deliver?', a: 'VST3 and AU for Windows and macOS — the stack Oriental Instrument ships on. AAX/CLAP can be scoped on request, not as a default build.' },
         { q: 'Is there a finished product I can see?', a: `Oriental Instrument — C++/JUCE oriental sampler, ${OI.instrumentCount} instruments, ${OI.presets} presets, Maqam quarter-tone tuning. In active development, closed beta to come.` },
         { q: 'How much does it cost?', a: 'It depends on scope. Tell me what you need and you get a quote for that project — no fixed published price.' }]
      : [{ q: 'Do you use WordPress or a CMS?', a: 'No — raw HTML5/CSS3/JS for speed and SEO. An editable content layer can be added if you need one.' },
         { q: 'Will it rank?', a: 'Built on a programmatic SEO architecture (semantic silos, JSON-LD, hreflang). This SEO cocoon is itself an example.' },
         { q: 'Do I own it?', a: 'Yes — full source, deployable anywhere (GitHub Pages, Cloudflare, your host).' }]
  };
}

/* ---------- HUB index pages ---------- */
function hubPage(key) {
  const h = arch.hubs[key];
  const spokeLinks = h.spokes.map(sp => {
    const label = sp.title || `${sp.genre} — ${sp.city}`;
    return `<div class="card"><h3><a href="${sp.path}">${esc(label)}</a></h3><p>${esc((sp.keywords && sp.keywords[0]) || sp.scene || '')}</p></div>`;
  }).join('');
  const isVst = key === 'vst-plugins', isBeats = key === 'beats';
  return {
    file: 'hub-' + key + '.html', lang: 'en', url: DOMAIN + h.path, includeProduct: isVst, serviceType: h.title,
    title: h.title + ' — DJBILBOX BEATS',
    meta: isVst ? 'Custom C++/JUCE VST3 & AU plugin development. Builder of Oriental Instrument. Explore services and request a quote.'
      : isBeats ? 'Professional beats — Hip-Hop, Afrotrap, Deep House, G-Funk, Oriental, Arabic — by city and scene. Lease, exclusive or custom.'
      : 'Web development for the music industry — artist landing pages, label sites, fast SEO-ready builds in Paris, Toulouse and worldwide.',
    h1: h.h1,
    intro: isVst ? `Custom audio plugins in <strong>C++ / JUCE</strong>. Real proof of work: <strong>${esc(OI.name)}</strong>, an oriental sampler with ${esc(OI.microtonal)}, shipping as VST3/AU plus iOS/Android.`
      : isBeats ? `Beats across <strong>${esc(facts.whatIsTrue.beatmaking.styles.join(', '))}</strong>, produced for the real sound of each city below.`
      : `Fast, owned, SEO-ready websites for artists, studios and labels — raw HTML5/CSS3/JS, no CMS bloat.`,
    ctaPrimary: 'Request a Quote', ctaSec: isVst ? 'JUCE proof of work' : 'Contact', ctaSecHref: isVst ? '/vst-plugins/juce-cpp-developer/' : '#lead',
    storeHeading: isBeats ? 'Beat store' : isVst ? 'Oriental Instrument — demo' : 'Portfolio demo',
    storeNote: isBeats ? 'Beatstars player embeds here.' : isVst ? 'Oriental Instrument demo embeds here (beta to come).' : 'Demo embeds here.',
    leadPlaceholder: 'What do you need?',
    crumb: `<a href="/">Home</a> › <span>${esc(h.h1)}</span>`,
    crumbItems: [{ name: 'Home', item: DOMAIN + '/' }, { name: h.h1, item: DOMAIN + h.path }],
    toc: [{ id: 'store', label: 'Demo' }, { id: 'explore', label: 'Explore' }, { id: 'faq', label: 'FAQ' }, { id: 'lead', label: 'Get a quote' }],
    body: `<h2 id="explore">Explore</h2><div class="grid">${spokeLinks}</div>`,
    related: `<a href="/vst-plugins/">VST plugins</a> · <a href="/beats/">Beats</a> · <a href="/web-dev-music/">Web dev</a>`,
    faq: isVst
      ? [{ q: 'What do you build?', a: `Custom VST3/AU plugins in C++/JUCE. Live product: ${OI.name} (${OI.instrumentCount} instruments, ${OI.presets} presets, Maqam tuning), in development.` },
         { q: 'Pricing?', a: 'On quote — scope-dependent, no fixed published figure.' }]
      : isBeats
      ? [{ q: 'What styles?', a: facts.whatIsTrue.beatmaking.styles.join(', ') + '.' },
         { q: 'Where do I buy?', a: 'Beats are on Beatstars; custom commissions by email.' }]
      : [{ q: 'What stack?', a: 'Raw HTML5, CSS3, JavaScript and Python. No CMS unless you ask for one.' },
         { q: 'Do I own the site?', a: 'Yes — full source, deploy anywhere.' }]
  };
}

/* ---------- run ---------- */
const written = [];
for (const s of arch.hubs.beats.spokes) { const p = beatPage(s); fs.writeFileSync(path.join(OUT, p.file), render(p)); written.push(p.url); }
for (const hub of ['vst-plugins', 'web-dev-music']) {
  for (const s of arch.hubs[hub].spokes) {
    if (s.isHeroPage) { written.push(DOMAIN + s.path + ' (hand-crafted hero, skipped)'); continue; }
    const p = servicePage(s, hub); fs.writeFileSync(path.join(OUT, p.file), render(p)); written.push(p.url);
  }
}
for (const key of ['beats', 'vst-plugins', 'web-dev-music']) { const p = hubPage(key); fs.writeFileSync(path.join(OUT, p.file), render(p)); written.push(p.url + ' (hub)'); }

/* sitemap */
const urls = written.filter(u => !u.includes('skipped')).map(u => u.replace(/ \(.*\)$/, '')).concat([DOMAIN + '/vst-plugins/juce-cpp-developer/']);
const uniq = [...new Set(urls)];
fs.writeFileSync(path.join(OUT, 'sitemap.xml'),
  `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n${uniq.map(u => `  <url><loc>${u}</loc><changefreq>weekly</changefreq><priority>0.8</priority></url>`).join('\n')}\n</urlset>`);

/* robots.txt */
fs.writeFileSync(path.join(OUT, 'robots.txt'),
  `User-agent: *\nAllow: /\nSitemap: ${DOMAIN}/sitemap.xml\n`);

/* IndexNow key (Bing/Yandex instant indexing) */
const KEY = require('crypto').randomBytes(16).toString('hex');
fs.writeFileSync(path.join(OUT, KEY + '.txt'), KEY);
fs.writeFileSync(path.join(OUT, 'indexnow-ping.js'),
  `#!/usr/bin/env node\n/* Ping IndexNow with all sitemap URLs. Run after deploy. */\nconst https=require('https');\nconst KEY=${JSON.stringify(KEY)};\nconst HOST=${JSON.stringify(DOMAIN.replace(/^https?:\/\//, ''))};\nconst urlList=${JSON.stringify(uniq)};\nconst body=JSON.stringify({host:HOST,key:KEY,keyLocation:${JSON.stringify(DOMAIN + '/' + KEY + '.txt')},urlList});\nconst req=https.request('https://api.indexnow.org/indexnow',{method:'POST',headers:{'Content-Type':'application/json','Content-Length':Buffer.byteLength(body)}},r=>{console.log('IndexNow status',r.statusCode);r.resume()});\nreq.on('error',e=>console.error(e.message));req.write(body);req.end();\n`);

console.log('Generated ' + written.length + ' pages -> ' + OUT);
written.forEach(u => console.log('  ' + u));
console.log('+ sitemap.xml (' + uniq.length + ' urls), robots.txt, IndexNow key + ping script');
