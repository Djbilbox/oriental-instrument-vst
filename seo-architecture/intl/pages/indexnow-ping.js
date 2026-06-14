#!/usr/bin/env node
/* Ping IndexNow with all sitemap URLs. Run after deploy. */
const https=require('https');
const KEY="03202c0723aaa2d545ce6dd7f3fda0f3";
const HOST="djbilboxbeats.com";
const urlList=["https://djbilboxbeats.com/beats/hip-hop-beats-atlanta/","https://djbilboxbeats.com/beats/trap-beats-los-angeles/","https://djbilboxbeats.com/beats/g-funk-beats-los-angeles/","https://djbilboxbeats.com/beats/afrotrap-loops-london/","https://djbilboxbeats.com/beats/afrotrap-beats-paris/","https://djbilboxbeats.com/beats/deep-house-producer-berlin/","https://djbilboxbeats.com/beats/arabic-oriental-beats-dubai/","https://djbilboxbeats.com/beats/uk-drill-beats-london/","https://djbilboxbeats.com/vst-plugins/custom-vst-plugin-creation/","https://djbilboxbeats.com/vst-plugins/audio-effects-plugin-architecture/","https://djbilboxbeats.com/vst-plugins/dsp-optimization-service/","https://djbilboxbeats.com/web-dev-music/artist-landing-page-creator/","https://djbilboxbeats.com/web-dev-music/music-label-web-developer/","https://djbilboxbeats.com/fr/web-dev-music/developpeur-web-musique-paris/","https://djbilboxbeats.com/fr/web-dev-music/developpeur-web-musique-toulouse/","https://djbilboxbeats.com/beats/","https://djbilboxbeats.com/vst-plugins/","https://djbilboxbeats.com/web-dev-music/","https://djbilboxbeats.com/vst-plugins/juce-cpp-developer/"];
const body=JSON.stringify({host:HOST,key:KEY,keyLocation:"https://djbilboxbeats.com/03202c0723aaa2d545ce6dd7f3fda0f3.txt",urlList});
const req=https.request('https://api.indexnow.org/indexnow',{method:'POST',headers:{'Content-Type':'application/json','Content-Length':Buffer.byteLength(body)}},r=>{console.log('IndexNow status',r.statusCode);r.resume()});
req.on('error',e=>console.error(e.message));req.write(body);req.end();
