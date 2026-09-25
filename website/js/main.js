import "./three-scene.js";

import {
    initScrollAnimations,
    initParallaxSections,
    initHeroMotion
} from "./animations.js";

import { initTerminal } from "./terminal.js";

document.addEventListener("DOMContentLoaded", () => {
    initScrollAnimations();
    initParallaxSections();
    initHeroMotion();
    initTerminal();

    console.log("ValaOS website initialized.");
});