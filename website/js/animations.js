export function initScrollAnimations() {
    const elements = document.querySelectorAll(
        ".content-section, " +
        ".terminal-section, " +
        ".status-section, " +
        ".download-section, " +
        ".feature-card, " +
        ".architecture-flow div, " +
        ".status div"
    );

    if (!elements.length) return;

    elements.forEach((element, index) => {
        element.classList.add("reveal");

        element.style.setProperty(
            "--reveal-delay",
            `${Math.min(index * 45, 250)}ms`
        );
    });

    const observer = new IntersectionObserver(
        (entries) => {
            entries.forEach((entry) => {
                if (!entry.isIntersecting) return;

                entry.target.classList.add("visible");
                observer.unobserve(entry.target);
            });
        },
        {
            threshold: 0.12,
            rootMargin: "0px 0px -8% 0px"
        }
    );

    elements.forEach((element) => {
        observer.observe(element);
    });
}

export function initParallaxSections() {
    const sections = document.querySelectorAll(
        ".content-section, .terminal-section, .status-section, .download-section"
    );

    if (!sections.length) return;

    let ticking = false;

    function updateParallax() {
        const viewportCenter = window.innerHeight / 2;

        sections.forEach((section) => {
            const rect = section.getBoundingClientRect();

            if (
                rect.bottom < -200 ||
                rect.top > window.innerHeight + 200
            ) {
                return;
            }

            const sectionCenter = rect.top + rect.height / 2;
            const distance = sectionCenter - viewportCenter;

            const offset = Math.max(
                -18,
                Math.min(18, distance * -0.025)
            );

            section.style.setProperty(
                "--section-parallax",
                `${offset}px`
            );
        });

        ticking = false;
    }

    function requestUpdate() {
        if (!ticking) {
            requestAnimationFrame(updateParallax);
            ticking = true;
        }
    }

    window.addEventListener("scroll", requestUpdate, {
        passive: true
    });

    window.addEventListener("resize", requestUpdate);

    requestUpdate();
}

export function initHeroMotion() {
    const hero = document.querySelector(".hero");

    if (!hero) return;

    let ticking = false;

    function updateHero() {
        const scrollY = window.scrollY;

        const opacity = Math.max(
            0,
            1 - scrollY / (window.innerHeight * 0.85)
        );

        const translateY = Math.min(
            90,
            scrollY * 0.22
        );

        hero.style.setProperty(
            "--hero-opacity",
            opacity
        );

        hero.style.setProperty(
            "--hero-offset",
            `${translateY}px`
        );

        ticking = false;
    }

    function requestUpdate() {
        if (!ticking) {
            requestAnimationFrame(updateHero);
            ticking = true;
        }
    }

    window.addEventListener("scroll", requestUpdate, {
        passive: true
    });

    window.addEventListener("resize", requestUpdate);

    requestUpdate();
}