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
const contactForm = document.getElementById("contactForm");
const contactStatus = document.getElementById("contactStatus");

if (contactForm) {
    contactForm.addEventListener("submit", async function (event) {
        event.preventDefault();

        const button = contactForm.querySelector("button");

        button.disabled = true;
        button.textContent = "SENDING...";

        const formData = new FormData(contactForm);

        try {
            const response = await fetch(
                "https://formsubmit.co/ajax/nikhilvala0005@gmail.com",
                {
                    method: "POST",
                    body: formData,
                    headers: {
                        "Accept": "application/json"
                    }
                }
            );

            const result = await response.json();

            if (response.ok && result.success) {
                contactForm.reset();

                contactStatus.style.display = "block";
                contactStatus.textContent =
                    "✓ Message sent successfully. Thank you for contacting me.";
                contactStatus.style.color = "#4ade80";

                button.textContent = "MESSAGE SENT";
            } else {
                throw new Error("Message could not be sent");
            }

        } catch (error) {
            contactStatus.style.display = "block";
            contactStatus.textContent =
                "Unable to send the message right now. Please try again or email me directly.";
            contactStatus.style.color = "#ff3344";

            button.disabled = false;
            button.textContent = "SEND MESSAGE";
        }
    });
}
const sponsorshipButton = document.getElementById("sponsorshipButton");
const contactSubject = document.querySelector('#contactForm input[name="subject"]');

if (sponsorshipButton && contactSubject) {
    sponsorshipButton.addEventListener("click", function () {
        contactSubject.value = "ValaOS Sponsorship / Support";
    });
}