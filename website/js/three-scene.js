import * as THREE from "https://cdn.jsdelivr.net/npm/three@0.180.0/build/three.module.js";

const canvas = document.getElementById("valaos-canvas");

if (!canvas) {
    throw new Error("ValaOS canvas not found.");
}

const scene = new THREE.Scene();

scene.background = new THREE.Color(0x03060b);

const camera = new THREE.PerspectiveCamera(
    42,
    window.innerWidth / window.innerHeight,
    0.1,
    1000
);

camera.position.set(0, 2.2, 12);

const renderer = new THREE.WebGLRenderer({
    canvas,
    antialias: true,
    alpha: true
});

const mobileDevice =
    window.innerWidth <= 768;

renderer.setPixelRatio(
    mobileDevice
        ? Math.min(window.devicePixelRatio, 1.5)
        : Math.min(window.devicePixelRatio, 2)
);

renderer.setSize(
    window.innerWidth,
    window.innerHeight
);

renderer.outputColorSpace =
    THREE.SRGBColorSpace;

renderer.toneMapping =
    THREE.ACESFilmicToneMapping;

renderer.toneMappingExposure =
    1.35;


/* =========================================
   LIGHTING
========================================= */

const hemisphere =
    new THREE.HemisphereLight(
        0xdde9ff,
        0x111827,
        1.2
    );

scene.add(hemisphere);


const keyLight =
    new THREE.DirectionalLight(
        0xffffff,
        3.2
    );

keyLight.position.set(
    5,
    8,
    8
);

scene.add(keyLight);


const redLight =
    new THREE.PointLight(
        0xff2638,
        20,
        32
    );

redLight.position.set(
    4,
    4,
    7
);

scene.add(redLight);


const goldLight =
    new THREE.PointLight(
        0xffb51b,
        10,
        28
    );

goldLight.position.set(
    -5,
    3,
    5
);

scene.add(goldLight);


const blueLight =
    new THREE.PointLight(
        0x5c8dff,
        8,
        30
    );

blueLight.position.set(
    -5,
    2,
    -6
);

scene.add(blueLight);


const flashLight =
    new THREE.PointLight(
        0xffffff,
        14,
        14
    );

scene.add(
    flashLight
);


/* =========================================
   HELPERS
========================================= */

function lerp(a, b, t) {
    return a + (b - a) * t;
}

function clamp(value, min, max) {
    return Math.max(
        min,
        Math.min(max, value)
    );
}

function smooth(value) {
    value = clamp(value, 0, 1);

    return value * value *
        (3 - 2 * value);
}

function progress(value, start, end) {
    return smooth(
        clamp(
            (value - start) /
            (end - start),
            0,
            1
        )
    );
}

function metal(
    color,
    roughness = 0.28,
    metalness = 0.9
) {
    return new THREE.MeshStandardMaterial({
        color,
        roughness,
        metalness
    });
}

function glow(
    color,
    opacity = 1
) {
    return new THREE.MeshBasicMaterial({
        color,
        transparent: true,
        opacity
    });
}


/* =========================================
   WORLD
========================================= */

const world =
    new THREE.Group();

scene.add(world);


/* =========================================
   SPACE BACKGROUND
========================================= */

const starCount =
    window.innerWidth <= 768
        ? 900
        : 1800;

const starPositions =
    new Float32Array(
        starCount * 3
    );

for (
    let i = 0;
    i < starCount;
    i++
) {

    starPositions[i * 3] =
        (Math.random() - 0.5) * 45;

    starPositions[i * 3 + 1] =
        (Math.random() - 0.5) * 28;

    starPositions[i * 3 + 2] =
        (Math.random() - 0.5) * 35;
}

const starGeometry =
    new THREE.BufferGeometry();

starGeometry.setAttribute(
    "position",
    new THREE.BufferAttribute(
        starPositions,
        3
    )
);

const starMaterial =
    new THREE.PointsMaterial({
        color: 0xd8e0ea,
        size: 0.018,
        transparent: true,
        opacity: 0.58
    });

const stars =
    new THREE.Points(
        starGeometry,
        starMaterial
    );

scene.add(stars);


/* Larger distant stars */

const glowStars =
    new THREE.Group();

scene.add(glowStars);

const glowStarCount =
    window.innerWidth <= 768
        ? 35
        : 70;

for (
    let i = 0;
    i < glowStarCount;
    i++
) {

    const star =
        new THREE.Mesh(
            new THREE.SphereGeometry(
                0.025 +
                Math.random() * 0.025,
                8,
                8
            ),
            glow(
                i % 8 === 0
                    ? 0xffc400
                    : 0xaec8ff,
                0.7
            )
        );

    star.position.set(
        (Math.random() - 0.5) * 30,
        (Math.random() - 0.5) * 18,
        -8 -
        Math.random() * 15
    );

    glowStars.add(star);
}


/* =========================================
   MAIN CHIP
========================================= */

const chipScene =
    new THREE.Group();

world.add(
    chipScene
);

chipScene.position.set(
    1.4,
    -0.5,
    0
);

chipScene.rotation.x =
    -0.13;


/* Main body */

const chipBody =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            6.8,
            0.58,
            4.8
        ),
        metal(
            0x363e47,
            0.23,
            0.95
        )
    );

chipScene.add(
    chipBody
);


/* Top plate */

const chipTop =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            6.45,
            0.18,
            4.45
        ),
        metal(
            0x5b646e,
            0.19,
            0.93
        )
    );

chipTop.position.y =
    0.37;

chipScene.add(
    chipTop
);


/* Silicon */

const silicon =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            3.9,
            0.18,
            2.95
        ),
        metal(
            0x181d23,
            0.16,
            0.9
        )
    );

silicon.position.y =
    0.54;

chipScene.add(
    silicon
);


/* Silicon border */

const siliconBorder =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            4.2,
            0.05,
            3.25
        ),
        glow(
            0xff3344,
            0.55
        )
    );

siliconBorder.position.y =
    0.65;

chipScene.add(
    siliconBorder
);


/* CPU */

const cpu =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            1.55,
            0.18,
            1.55
        ),
        new THREE.MeshStandardMaterial({
            color: 0x701925,
            emissive: 0xff2035,
            emissiveIntensity: 2.5,
            metalness: 0.9,
            roughness: 0.15
        })
    );

cpu.position.y =
    0.72;

chipScene.add(
    cpu
);


/* CPU glass */

const cpuGlass =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            1.25,
            0.04,
            1.25
        ),
        new THREE.MeshPhysicalMaterial({
            color: 0xff3344,
            emissive: 0x8e0615,
            emissiveIntensity: 1.8,
            metalness: 0.5,
            roughness: 0.1,
            transparent: true,
            opacity: 0.85
        })
    );

cpuGlass.position.y =
    0.83;

chipScene.add(
    cpuGlass
);


const cpuLight =
    new THREE.PointLight(
        0xff2035,
        7,
        5
    );

cpuLight.position.y =
    1.4;

chipScene.add(
    cpuLight
);


/* =========================================
   CHIP PINS
========================================= */

const pinMaterial =
    metal(
        0xd1a23b,
        0.17,
        1
    );

for (
    let i = -13;
    i <= 13;
    i++
) {

    const front =
        new THREE.Mesh(
            new THREE.BoxGeometry(
                0.08,
                0.18,
                0.28
            ),
            pinMaterial
        );

    front.position.set(
        i * 0.22,
        -0.25,
        2.48
    );

    chipScene.add(
        front
    );

    const back =
        front.clone();

    back.position.z =
        -2.48;

    chipScene.add(
        back
    );
}

for (
    let i = -9;
    i <= 9;
    i++
) {

    const left =
        new THREE.Mesh(
            new THREE.BoxGeometry(
                0.28,
                0.18,
                0.08
            ),
            pinMaterial
        );

    left.position.set(
        -3.48,
        -0.25,
        i * 0.22
    );

    chipScene.add(
        left
    );

    const right =
        left.clone();

    right.position.x =
        3.48;

    chipScene.add(
        right
    );
}


/* =========================================
   CHIP CIRCUITS
========================================= */

const redTrace =
    glow(
        0xff3344,
        0.72
    );

const goldTrace =
    glow(
        0xffc400,
        0.7
    );

for (
    let i = 0;
    i < 45;
    i++
) {

    const horizontal =
        i % 2 === 0;

    const length =
        0.4 +
        Math.random() * 1.6;

    const trace =
        new THREE.Mesh(
            new THREE.BoxGeometry(
                horizontal
                    ? length
                    : 0.035,
                0.025,
                horizontal
                    ? 0.035
                    : length
            ),
            i % 7 === 0
                ? goldTrace
                : redTrace
        );

    trace.position.set(
        (Math.random() - 0.5) * 5.7,
        0.68,
        (Math.random() - 0.5) * 3.8
    );

    chipScene.add(
        trace
    );
}


/* =========================================
   DATA PARTICLES
========================================= */

const dataParticles = [];

for (
    let i = 0;
    i < 18;
    i++
) {

    const particle =
        new THREE.Mesh(
            new THREE.SphereGeometry(
                0.035,
                10,
                10
            ),
            glow(
                i % 4 === 0
                    ? 0xffc400
                    : 0xff3344,
                0.95
            )
        );

    particle.position.set(
        -2.8 +
        Math.random() * 5.6,
        0.73,
        -1.8 +
        Math.random() * 3.6
    );

    chipScene.add(
        particle
    );

    dataParticles.push({
        object: particle,
        speed:
            0.2 +
            Math.random() * 0.5,
        offset:
            Math.random()
    });
}


/* =========================================
   UBUNTU TERMINAL SCENE
========================================= */

const ubuntuScene =
    new THREE.Group();

world.add(
    ubuntuScene
);

ubuntuScene.position.set(
    0,
    0,
    0
);


/* Terminal outer frame */

const terminalFrame =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            7.4,
            4.5,
            0.22
        ),
        metal(
            0x252b32,
            0.25,
            0.9
        )
    );

ubuntuScene.add(
    terminalFrame
);


/* Terminal screen */

const terminalScreen =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            7.05,
            4.12,
            0.08
        ),
        new THREE.MeshStandardMaterial({
            color: 0x10151b,
            emissive: 0x061018,
            emissiveIntensity: 0.7,
            roughness: 0.3,
            metalness: 0.4
        })
    );

terminalScreen.position.z =
    0.15;

ubuntuScene.add(
    terminalScreen
);


/* Terminal header */

const terminalHeader =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            7.05,
            0.42,
            0.09
        ),
        metal(
            0x30363d,
            0.25,
            0.7
        )
    );

terminalHeader.position.set(
    0,
    1.85,
    0.22
);

ubuntuScene.add(
    terminalHeader
);


/* Header lights */

for (
    let i = 0;
    i < 3;
    i++
) {

    const dot =
        new THREE.Mesh(
            new THREE.SphereGeometry(
                0.055,
                12,
                12
            ),
            glow(
                i === 0
                    ? 0xff5f56
                    : i === 1
                    ? 0xffbd2e
                    : 0x27c93f,
                0.95
            )
        );

    dot.position.set(
        -3.05 +
        i * 0.2,
        1.85,
        0.3
    );

    ubuntuScene.add(
        dot
    );
}


/* Terminal command lines */

const terminalLines = [];

const terminalData = [
    "Ubuntu 24.04 LTS",
    "nehal@valaos:~/ValaOS$ make",
    "gcc -m32 -ffreestanding",
    "nasm -f elf32 kernel/boot.asm",
    "ld -m elf_i386 -T linker.ld",
    "Build complete.",
    "",
    "nehal@valaos:~/ValaOS$ make run",
    "Starting ValaOS...",
    "ValaOS 2.0",
    "SYSTEM ONLINE"
];

for (
    let i = 0;
    i < terminalData.length;
    i++
) {

    const geometry =
        new THREE.PlaneGeometry(
            0.001,
            0.001
        );

    const material =
        new THREE.MeshBasicMaterial({
            transparent: true,
            opacity: 0
        });

    const line =
        new THREE.Mesh(
            geometry,
            material
        );

    line.position.set(
        -3.1,
        1.35 -
        i * 0.28,
        0.29
    );

    ubuntuScene.add(
        line
    );

    terminalLines.push({
        object: line,
        text: terminalData[i]
    });
}


/* =========================================
   KERNEL
========================================= */

const kernelScene =
    new THREE.Group();

world.add(
    kernelScene
);

const kernel =
    new THREE.Mesh(
        new THREE.IcosahedronGeometry(
            1.45,
            3
        ),
        new THREE.MeshStandardMaterial({
            color: 0x424b54,
            emissive: 0x4b0c13,
            emissiveIntensity: 1.2,
            metalness: 0.92,
            roughness: 0.2
        })
    );

kernelScene.add(
    kernel
);


const kernelCore =
    new THREE.Mesh(
        new THREE.IcosahedronGeometry(
            0.78,
            2
        ),
        new THREE.MeshStandardMaterial({
            color: 0x7a1c28,
            emissive: 0xff2438,
            emissiveIntensity: 2.5,
            metalness: 0.85,
            roughness: 0.15
        })
    );

kernelScene.add(
    kernelCore
);


const kernelWire =
    new THREE.Mesh(
        new THREE.IcosahedronGeometry(
            1.85,
            2
        ),
        new THREE.MeshBasicMaterial({
            color: 0xb9c3ce,
            wireframe: true,
            transparent: true,
            opacity: 0.32
        })
    );

kernelScene.add(
    kernelWire
);


/* Kernel modules */

const modules = [];

const modulePositions = [
    [-2.8, 1.4],
    [2.8, 1.4],
    [-2.8, -1.4],
    [2.8, -1.4]
];

modulePositions.forEach(
    (position, index) => {

        const module =
            new THREE.Mesh(
                new THREE.BoxGeometry(
                    0.78,
                    0.78,
                    0.78
                ),
                metal(
                    index === 1
                        ? 0x741d29
                        : 0x454e57,
                    0.25,
                    0.9
                )
            );

        module.position.set(
            position[0],
            position[1],
            0
        );

        kernelScene.add(
            module
        );

        modules.push(
            module
        );
    }
);


/* =========================================
   MEMORY
========================================= */

const memoryScene =
    new THREE.Group();

world.add(
    memoryScene
);

const memoryGrid =
    new THREE.Group();

memoryScene.add(
    memoryGrid
);

const memoryCells = [];

for (
    let x = -6;
    x <= 6;
    x++
) {

    for (
        let z = -4;
        z <= 4;
        z++
    ) {

        const active =
            Math.random() > 0.84;

        const cell =
            new THREE.Mesh(
                new THREE.BoxGeometry(
                    0.25,
                    0.18,
                    0.25
                ),
                metal(
                    active
                        ? 0x741c28
                        : 0x3b444d,
                    0.28,
                    0.85
                )
            );

        cell.position.set(
            x * 0.4,
            0,
            z * 0.4
        );

        memoryGrid.add(
            cell
        );

        memoryCells.push({
            object: cell,
            x: x,
            z: z
        });
    }
}


/* =========================================
   SCHEDULER
========================================= */

const schedulerScene =
    new THREE.Group();

world.add(
    schedulerScene
);

const schedulerNodes = [];

const schedulerPositions = [
    [-2.8, 0, 0],
    [0, 1.7, 0],
    [2.8, 0, 0]
];

schedulerPositions.forEach(
    (position, index) => {

        const node =
            new THREE.Mesh(
                new THREE.BoxGeometry(
                    0.9,
                    0.9,
                    0.9
                ),
                metal(
                    index === 1
                        ? 0x781f2a
                        : 0x454e57,
                    0.25,
                    0.9
                )
            );

        node.position.set(
            position[0],
            position[1],
            position[2]
        );

        schedulerScene.add(
            node
        );

        schedulerNodes.push(
            node
        );
    }
);


const timerPulse =
    new THREE.Mesh(
        new THREE.SphereGeometry(
            0.14,
            16,
            16
        ),
        glow(
            0xffc400,
            1
        )
    );

schedulerScene.add(
    timerPulse
);


/* =========================================
   RING 0 / RING 3
========================================= */

const securityScene =
    new THREE.Group();

world.add(
    securityScene
);


const ring0 =
    new THREE.Mesh(
        new THREE.TorusGeometry(
            3.1,
            0.075,
            16,
            160
        ),
        glow(
            0xff3344,
            0.8
        )
    );

ring0.rotation.x =
    Math.PI / 2;

securityScene.add(
    ring0
);


const ring3 =
    new THREE.Mesh(
        new THREE.TorusGeometry(
            1.55,
            0.06,
            16,
            160
        ),
        glow(
            0xffc400,
            0.8
        )
    );

ring3.rotation.x =
    Math.PI / 2;

securityScene.add(
    ring3
);


const securityCore =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            0.75,
            0.75,
            0.75
        ),
        metal(
            0x4b555e,
            0.2,
            0.92
        )
    );

securityScene.add(
    securityCore
);


const syscall =
    new THREE.Mesh(
        new THREE.SphereGeometry(
            0.13,
            16,
            16
        ),
        glow(
            0xffffff,
            1
        )
    );

securityScene.add(
    syscall
);


/* =========================================
   VFS
========================================= */

const vfsScene =
    new THREE.Group();

world.add(
    vfsScene
);

const vfsPositions = [
    [0, 2.5, 0],
    [-2.8, 0.7, 0],
    [0, 0.7, 0],
    [2.8, 0.7, 0],
    [-3.5, -1.3, 0],
    [-1.8, -1.3, 0],
    [2.8, -1.3, 0]
];

const vfsNodes = [];

vfsPositions.forEach(
    (position, index) => {

        const size =
            index === 0
                ? 0.9
                : 0.58;

        const node =
            new THREE.Mesh(
                new THREE.BoxGeometry(
                    size,
                    size,
                    size
                ),
                metal(
                    index === 0
                        ? 0x721c28
                        : 0x454e57,
                    0.25,
                    0.9
                )
            );

        node.position.set(
            position[0],
            position[1],
            position[2]
        );

        vfsScene.add(
            node
        );

        vfsNodes.push(
            node
        );
    }
);


/* =========================================
   FINAL CHIP
========================================= */

const finalScene =
    new THREE.Group();

world.add(
    finalScene
);

const finalChip =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            3.8,
            0.5,
            2.9
        ),
        metal(
            0x515b65,
            0.22,
            0.95
        )
    );

finalScene.add(
    finalChip
);


const finalCore =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            1.25,
            0.18,
            1.25
        ),
        new THREE.MeshStandardMaterial({
            color: 0x781d29,
            emissive: 0xff2438,
            emissiveIntensity: 2.5,
            metalness: 0.88,
            roughness: 0.16
        })
    );

finalCore.position.y =
    0.32;

finalScene.add(
    finalCore
);

/* =========================================
   DEVELOPMENT SEQUENCE
========================================= */

const developmentScene = new THREE.Group();
world.add(developmentScene);

developmentScene.position.set(0, 0.2, 0);

function createDevScreen(title, lines, accent = 0xff3344) {
    const group = new THREE.Group();

    const frame = new THREE.Mesh(
        new THREE.BoxGeometry(5.8, 3.5, 0.18),
        metal(0x20262d, 0.24, 0.92)
    );

    group.add(frame);

    const screenCanvas = document.createElement("canvas");
    screenCanvas.width = 1200;
    screenCanvas.height = 700;

    const ctx = screenCanvas.getContext("2d");

    ctx.fillStyle = "#070a0e";
    ctx.fillRect(0, 0, 1200, 700);

    ctx.fillStyle = "#161b22";
    ctx.fillRect(0, 0, 1200, 72);

    const accentHex =
        "#" + accent.toString(16).padStart(6, "0");

    ctx.fillStyle = accentHex;
    ctx.fillRect(0, 68, 1200, 4);

    ctx.font = "bold 30px Arial";
    ctx.fillStyle = "#ffffff";
    ctx.fillText(title, 42, 47);

    ctx.fillStyle = "#ff5f56";
    ctx.beginPath();
    ctx.arc(1100, 36, 10, 0, Math.PI * 2);
    ctx.fill();

    ctx.fillStyle = "#ffbd2e";
    ctx.beginPath();
    ctx.arc(1132, 36, 10, 0, Math.PI * 2);
    ctx.fill();

    ctx.fillStyle = "#27c93f";
    ctx.beginPath();
    ctx.arc(1164, 36, 10, 0, Math.PI * 2);
    ctx.fill();

    ctx.font = "26px Consolas, monospace";

    lines.forEach((line, index) => {
        const y = 125 + index * 62;

        if (line.includes("SUCCESS") || line.includes("COMPLETE") || line.includes("READY")) {
            ctx.fillStyle = "#76c893";
        } else if (
            line.startsWith("PS ") ||
            line.includes("nehal@valaos")
        ) {
            ctx.fillStyle = accentHex;
        } else {
            ctx.fillStyle = "#d7dee7";
        }

        ctx.fillText(line, 42, y);
    });

    const texture =
        new THREE.CanvasTexture(screenCanvas);

    texture.colorSpace = THREE.SRGBColorSpace;

    const screen = new THREE.Mesh(
        new THREE.PlaneGeometry(5.45, 3.05),
        new THREE.MeshBasicMaterial({
            map: texture,
            transparent: false
        })
    );

    screen.position.z = 0.16;

    group.add(screen);

    const frameGlow = new THREE.Mesh(
        new THREE.BoxGeometry(5.45, 0.035, 0.035),
        glow(accent, 0.85)
    );

    frameGlow.position.set(
        0,
        -1.43,
        0.24
    );

    group.add(frameGlow);

    group.userData.title = title;
    group.userData.lines = lines;

    return group;
}
const powerShellScreen = createDevScreen(
    "PowerShell",
    [
        "PS D:\\ValaOS> make",
        "",
        "NASM  boot.asm",
        "GCC   kernel.c",
        "LD    kernel.bin",
        "",
        "BUILD SUCCESSFUL"
    ],
    0x5c8dff
);

powerShellScreen.position.set(
    -7,
    0.2,
    1
);

powerShellScreen.rotation.y = 0.22;

developmentScene.add(
    powerShellScreen
);


const vscodeScreen = createDevScreen(
    "VS Code",
    [
        "VALAOS PROJECT",
        "",
        "kernel/",
        "  kernel.c",
        "  paging.c",
        "  task.c",
        "  syscall.c",
        "",
        "PROJECT READY"
    ],
    0x3fa9f5
);

vscodeScreen.position.set(
    0,
    0.2,
    0
);

developmentScene.add(
    vscodeScreen
);


const ubuntuDevScreen = createDevScreen(
    "Ubuntu / WSL",
    [
        "Ubuntu 24.04 LTS",
        "",
        "nehal@valaos:~/ValaOS$",
        "make iso",
        "",
        "Creating ValaOS.iso",
        "",
        "ISO BUILD COMPLETE"
    ],
    0xffb51b
);

ubuntuDevScreen.position.set(
    7,
    0.2,
    1
);

ubuntuDevScreen.rotation.y = -0.22;

developmentScene.add(
    ubuntuDevScreen
);


const developmentScreens = [
    powerShellScreen,
    vscodeScreen,
    ubuntuDevScreen
];

/* =========================================
   GRUB BOOT SEQUENCE
========================================= */

const bootScene = new THREE.Group();

world.add(bootScene);

bootScene.position.set(0, 0.15, 0);

const bootCanvas = document.createElement("canvas");

bootCanvas.width = 1200;
bootCanvas.height = 700;

const bootCtx = bootCanvas.getContext("2d");

bootCtx.fillStyle = "#020304";
bootCtx.fillRect(0, 0, 1200, 700);

bootCtx.strokeStyle = "#303840";
bootCtx.lineWidth = 2;
bootCtx.strokeRect(40, 40, 1120, 620);

bootCtx.font = "bold 46px Arial";
bootCtx.fillStyle = "#ffffff";
bootCtx.textAlign = "center";

bootCtx.fillText(
    "VALAOS 2.0",
    600,
    150
);

bootCtx.font = "28px Consolas, monospace";
bootCtx.fillStyle = "#ff3344";

bootCtx.fillText(
    "GRUB BOOTLOADER",
    600,
    215
);

bootCtx.font = "24px Consolas, monospace";
bootCtx.fillStyle = "#d7dee7";

bootCtx.fillText(
    "Loading kernel.bin...",
    600,
    285
);

bootCtx.strokeStyle = "#343b43";
bootCtx.lineWidth = 12;

bootCtx.beginPath();
bootCtx.moveTo(250, 360);
bootCtx.lineTo(950, 360);
bootCtx.stroke();

bootCtx.strokeStyle = "#ff3344";
bootCtx.lineWidth = 12;

bootCtx.beginPath();
bootCtx.moveTo(250, 360);
bootCtx.lineTo(820, 360);
bootCtx.stroke();

bootCtx.font = "22px Consolas, monospace";
bootCtx.fillStyle = "#76c893";

bootCtx.fillText(
    "[ OK ] kernel.bin loaded",
    600,
    440
);

bootCtx.fillText(
    "[ OK ] Protected mode",
    600,
    485
);

bootCtx.fillStyle = "#ffffff";

bootCtx.fillText(
    "Transferring control to kernel...",
    600,
    550
);

const bootTexture =
    new THREE.CanvasTexture(bootCanvas);

bootTexture.colorSpace =
    THREE.SRGBColorSpace;

const bootFrame =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            7.2,
            4.4,
            0.22
        ),
        metal(
            0x1b2026,
            0.22,
            0.9
        )
    );

bootScene.add(
    bootFrame
);

const bootScreen =
    new THREE.Mesh(
        new THREE.PlaneGeometry(
            6.9,
            4.02
        ),
        new THREE.MeshBasicMaterial({
            map: bootTexture
        })
    );

bootScreen.position.z = 0.14;

bootScene.add(
    bootScreen
);

const bootGlow =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            5.8,
            0.035,
            0.035
        ),
        glow(
            0xff3344,
            0.9
        )
    );

bootGlow.position.set(
    0,
    -1.78,
    0.25
);

bootScene.add(
    bootGlow
);
/* =========================================
   KERNEL INITIALIZATION HUD
========================================= */

const kernelHudScene = new THREE.Group();

world.add(kernelHudScene);

kernelHudScene.position.set(
    0,
    0.1,
    0
);

const hudCanvas = document.createElement("canvas");

hudCanvas.width = 1100;
hudCanvas.height = 760;

const hudCtx = hudCanvas.getContext("2d");

hudCtx.fillStyle = "#05070a";
hudCtx.fillRect(
    0,
    0,
    hudCanvas.width,
    hudCanvas.height
);

hudCtx.strokeStyle = "#343b43";
hudCtx.lineWidth = 3;

hudCtx.strokeRect(
    35,
    35,
    1030,
    690
);

hudCtx.fillStyle = "#ff3344";

hudCtx.font =
    "bold 42px Consolas, monospace";

hudCtx.fillText(
    "VALAOS KERNEL",
    75,
    105
);

hudCtx.fillStyle = "#68727d";

hudCtx.font =
    "22px Consolas, monospace";

hudCtx.fillText(
    "INITIALIZATION SEQUENCE",
    75,
    145
);

const kernelStatusLines = [
    "GDT initialized",
    "Protected mode",
    "Memory manager",
    "Paging subsystem",
    "Page fault handler",
    "Interrupt descriptor table",
    "Hardware timer",
    "Task scheduler"
];

kernelStatusLines.forEach(
    (line, index) => {

        const y =
            205 +
            index * 55;

        hudCtx.fillStyle =
            "#76c893";

        hudCtx.font =
            "22px Consolas, monospace";

        hudCtx.fillText(
            "[ OK ]",
            80,
            y
        );

        hudCtx.fillStyle =
            "#d7dee7";

        hudCtx.fillText(
            line,
            200,
            y
        );
    }
);

hudCtx.fillStyle =
    "#ff3344";

hudCtx.font =
    "bold 25px Consolas, monospace";

hudCtx.fillText(
    "KERNEL INITIALIZED",
    75,
    675
);

const hudTexture =
    new THREE.CanvasTexture(
        hudCanvas
    );

hudTexture.colorSpace =
    THREE.SRGBColorSpace;

const kernelHudFrame =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            6.5,
            4.55,
            0.18
        ),
        metal(
            0x1c2228,
            0.24,
            0.9
        )
    );

kernelHudScene.add(
    kernelHudFrame
);

const kernelHudScreen =
    new THREE.Mesh(
        new THREE.PlaneGeometry(
            6.25,
            4.3
        ),
        new THREE.MeshBasicMaterial({
            map: hudTexture
        })
    );

kernelHudScreen.position.z =
    0.12;

kernelHudScene.add(
    kernelHudScreen
);

const kernelHudGlow =
    new THREE.Mesh(
        new THREE.BoxGeometry(
            5.2,
            0.035,
            0.035
        ),
        glow(
            0xff3344,
            0.85
        )
    );

kernelHudGlow.position.set(
    0,
    -1.88,
    0.22
);

kernelHudScene.add(
    kernelHudGlow
);
/* =========================================
   RUNTIME DATA FLOW
========================================= */

const runtimeFlowScene = new THREE.Group();

world.add(runtimeFlowScene);

runtimeFlowScene.position.set(
    0,
    0,
    0
);

const flowNodes = [
    {
        object: kernelScene,
        position: new THREE.Vector3(0, 0, 0)
    },
    {
        object: memoryScene,
        position: new THREE.Vector3(-3.5, 0.5, 0)
    },
    {
        object: schedulerScene,
        position: new THREE.Vector3(3.5, 0.5, 0)
    },
    {
        object: securityScene,
        position: new THREE.Vector3(0, -2.2, 0)
    },
    {
        object: vfsScene,
        position: new THREE.Vector3(0, 2.2, 0)
    }
];

const flowParticles = [];

const flowParticleMaterial =
    glow(
        0xff3344,
        1
    );

const flowConnections = [
    [0, 1],
    [0, 2],
    [1, 3],
    [2, 3],
    [3, 4]
];

flowConnections.forEach(
    (connection) => {

        const start =
            flowNodes[
                connection[0]
            ].position;

        const end =
            flowNodes[
                connection[1]
            ].position;

        const points = [
            start,
            end
        ];

        const geometry =
            new THREE.BufferGeometry()
                .setFromPoints(points);

        const material =
            new THREE.LineBasicMaterial({
                color: 0xff3344,
                transparent: true,
                opacity: 0.28
            });

        const line =
            new THREE.Line(
                geometry,
                material
            );

        runtimeFlowScene.add(
            line
        );
    }
);

for (
    let i = 0;
    i < 12;
    i++
) {

    const particle =
        new THREE.Mesh(
            new THREE.SphereGeometry(
                0.055,
                12,
                12
            ),
            flowParticleMaterial
        );

    runtimeFlowScene.add(
        particle
    );

    flowParticles.push({
        object: particle,
        path:
            i %
            flowConnections.length,
        offset:
            Math.random()
    });
}
/* =========================================
   FINAL SYSTEM ONLINE
========================================= */

const systemOnlineScene = new THREE.Group();

world.add(systemOnlineScene);

const onlineCore = new THREE.Mesh(
    new THREE.IcosahedronGeometry(1.25, 3),
    new THREE.MeshStandardMaterial({
        color: 0x090b10,
        metalness: 0.85,
        roughness: 0.2,
        emissive: 0x330008,
        emissiveIntensity: 1.5
    })
);

systemOnlineScene.add(onlineCore);

const onlineWire = new THREE.Mesh(
    new THREE.IcosahedronGeometry(1.42, 2),
    new THREE.MeshBasicMaterial({
        color: 0xff3344,
        wireframe: true,
        transparent: true,
        opacity: 0.55
    })
);

systemOnlineScene.add(onlineWire);

const onlineGlow = new THREE.PointLight(
    0xff3344,
    5,
    10
);

onlineGlow.position.set(
    0,
    0,
    1
);

systemOnlineScene.add(
    onlineGlow
);

const onlineRing = new THREE.Mesh(
    new THREE.TorusGeometry(
        1.9,
        0.025,
        12,
        96
    ),
    new THREE.MeshBasicMaterial({
        color: 0xff3344,
        transparent: true,
        opacity: 0.7
    })
);

onlineRing.rotation.x =
    Math.PI / 2;

systemOnlineScene.add(
    onlineRing
);


/* STATUS PANEL */

const onlineCanvas =
    document.createElement("canvas");

onlineCanvas.width = 900;
onlineCanvas.height = 700;

const onlineCtx =
    onlineCanvas.getContext("2d");

onlineCtx.fillStyle =
    "rgba(3,5,9,0.96)";

onlineCtx.fillRect(
    0,
    0,
    900,
    700
);

onlineCtx.font =
    "bold 34px monospace";

onlineCtx.fillStyle =
    "#ff3344";

onlineCtx.fillText(
    "VALAOS 2.0",
    55,
    65
);

onlineCtx.font =
    "20px monospace";

const statusLines = [
    "KERNEL          [ PASS ]",
    "MEMORY          [ PASS ]",
    "PAGING          [ PASS ]",
    "INTERRUPTS      [ PASS ]",
    "SCHEDULER       [ PASS ]",
    "RING 3          [ PASS ]",
    "SYSTEM CALLS    [ PASS ]",
    "PERSISTENT VFS  [ PASS ]"
];

statusLines.forEach(
    (line, index) => {

        onlineCtx.fillStyle =
            "#76c893";

        onlineCtx.fillText(
            line,
            55,
            130 + index * 52
        );
    }
);

onlineCtx.fillStyle =
    "#ffffff";

onlineCtx.font =
    "bold 32px monospace";

onlineCtx.fillText(
    "SYSTEM ONLINE",
    55,
    585
);

onlineCtx.font =
    "18px monospace";

onlineCtx.fillStyle =
    "#999999";

onlineCtx.fillText(
    "32-BIT x86 • CUSTOM OPERATING SYSTEM",
    55,
    625
);

const onlineTexture =
    new THREE.CanvasTexture(
        onlineCanvas
    );

const onlinePanel =
    new THREE.Mesh(
        new THREE.PlaneGeometry(
            6.8,
            5.3
        ),
        new THREE.MeshBasicMaterial({
            map: onlineTexture,
            transparent: true,
            opacity: 0
        })
    );

onlinePanel.position.set(
    -4.8,
    0,
    -0.5
);

systemOnlineScene.add(
    onlinePanel
);


/* FINAL CORE LABEL */

const finalCanvas =
    document.createElement("canvas");

finalCanvas.width = 700;
finalCanvas.height = 180;

const finalCtx =
    finalCanvas.getContext("2d");

finalCtx.textAlign =
    "center";

finalCtx.font =
    "bold 48px monospace";

finalCtx.fillStyle =
    "#ffffff";

finalCtx.fillText(
    "VALAOS 2.0",
    350,
    65
);

finalCtx.font =
    "bold 28px monospace";

finalCtx.fillStyle =
    "#76c893";

finalCtx.fillText(
    "SYSTEM ONLINE",
    350,
    115
);

const finalTexture =
    new THREE.CanvasTexture(
        finalCanvas
    );

const finalLabel =
    new THREE.Mesh(
        new THREE.PlaneGeometry(
            5.5,
            1.4
        ),
        new THREE.MeshBasicMaterial({
            map: finalTexture,
            transparent: true,
            opacity: 0
        })
    );

finalLabel.position.set(
    2.8,
    -3.0,
    0
);

systemOnlineScene.add(
    finalLabel
);
/* =========================================
   SCROLL
========================================= */

let scrollTarget = 0;
let scrollValue = 0;

window.addEventListener(
    "scroll",
    () => {

        const maxScroll =
            document.documentElement
                .scrollHeight -
            window.innerHeight;

        if (maxScroll <= 0) {
            scrollTarget = 0;
            return;
        }

        scrollTarget =
            window.scrollY /
            maxScroll;
    },
    {
        passive: true
    }
);


/* =========================================
   MOUSE
========================================= */

let mouseX = 0;
let mouseY = 0;

window.addEventListener(
    "mousemove",
    (event) => {

        mouseX =
            (
                event.clientX /
                window.innerWidth -
                0.5
            ) * 2;

        mouseY =
            (
                event.clientY /
                window.innerHeight -
                0.5
            ) * 2;
    }
);


/* =========================================
   CLOCK
========================================= */

const clock =
    new THREE.Clock();


/* =========================================
   ANIMATION
========================================= */

function animate() {

    requestAnimationFrame(
        animate
    );

    const time =
        clock.getElapsedTime();


    /* Smooth scroll */

    scrollValue +=
        (
            scrollTarget -
            scrollValue
        ) * 0.055;


    /* =====================================
       SPACE
    ===================================== */

    stars.rotation.y =
        time * 0.006;

    stars.rotation.x =
        Math.sin(time * 0.08) *
        0.01;

    glowStars.rotation.y =
        -time * 0.004;

/* =====================================
   KERNEL INITIALIZATION HUD
===================================== */

const kernelHudProgress =
    progress(
        scrollValue,
        0.23,
        0.38
    );

kernelHudScene.scale.setScalar(
    lerp(
        0.02,
        1,
        kernelHudProgress
    )
);

kernelHudScene.position.z =
    lerp(
        4,
        -0.7,
        kernelHudProgress
    );

kernelHudScene.rotation.y =
    lerp(
        0.08,
        0,
        kernelHudProgress
    );

kernelHudGlow.material.opacity =
    0.5 +
    Math.sin(time * 3) * 0.25;
    /* =====================================
   RUNTIME DATA FLOW
===================================== */

const runtimeProgress =
    progress(
        scrollValue,
        0.48,
        0.92
    );

runtimeFlowScene.scale.setScalar(
    lerp(
        0.02,
        1,
        runtimeProgress
    )
);

flowParticles.forEach(
    (particle) => {

        const connection =
            flowConnections[
                particle.path
            ];

        const start =
            flowNodes[
                connection[0]
            ].position;

        const end =
            flowNodes[
                connection[1]
            ].position;

        const travel =
            (
                time * 0.45 +
                particle.offset
            ) % 1;

        particle.object.position.lerpVectors(
            start,
            end,
            travel
        );
    }
);
    /* =====================================
       CINEMATIC LIGHT
    ===================================== */

    flashLight.position.x =
        Math.sin(time * 0.4) *
        6;

    flashLight.position.y =
        3 +
        Math.sin(time * 0.7) *
        1.5;

    flashLight.position.z =
        4 +
        Math.cos(time * 0.35) *
        3;

    redLight.intensity =
        20 +
        Math.sin(time * 1.7) *
        3;

    goldLight.intensity =
        10 +
        Math.sin(time * 1.1) *
        2;

/* =====================================
   DEVELOPMENT SEQUENCE
===================================== */

const developmentProgress =
    progress(
        scrollValue,
        0.035,
        0.16
    );

developmentScene.scale.setScalar(
    lerp(
        0.02,
        1,
        developmentProgress
    )
);

developmentScene.position.z =
    lerp(
        5,
        -0.8,
        developmentProgress
    );

developmentScene.rotation.y =
    lerp(
        0.12,
        0,
        developmentProgress
    );

developmentScreens.forEach(
    (screen, index) => {

        const offset =
            Math.sin(
                time * 0.7 +
                index
            ) * 0.025;

        screen.position.y =
            0.2 + offset;
    }
);
/* =====================================
   GRUB BOOT SEQUENCE
===================================== */

const bootProgress =
    progress(
        scrollValue,
        0.14,
        0.25
    );

bootScene.scale.setScalar(
    lerp(
        0.02,
        1,
        bootProgress
    )
);

bootScene.position.z =
    lerp(
        4,
        -0.5,
        bootProgress
    );

bootScene.rotation.y =
    lerp(
        0.08,
        0,
        bootProgress
    );

bootGlow.material.opacity =
    0.45 +
    Math.sin(time * 3) * 0.25;

    /* =====================================
   FINAL SYSTEM ONLINE
===================================== */

const onlineProgress =
    progress(
        scrollValue,
        0.90,
        1.0
    );

systemOnlineScene.scale.setScalar(
    lerp(
        0.02,
        1,
        onlineProgress
    )
);

systemOnlineScene.position.z =
    lerp(
        4,
        -1,
        onlineProgress
    );

systemOnlineScene.rotation.y =
    lerp(
        0.18,
        0,
        onlineProgress
    );

onlineCore.rotation.x +=
    0.0025;

onlineCore.rotation.y +=
    0.004;

onlineWire.rotation.x -=
    0.0015;

onlineWire.rotation.y +=
    0.002;

onlineRing.rotation.z +=
    0.004;

const pulse =
    1 +
    Math.sin(time * 3.5) *
    0.08;

onlineCore.scale.setScalar(
    pulse
);

onlineGlow.intensity =
    4.5 +
    Math.sin(time * 3.5) *
    1.5;

onlinePanel.material.opacity =
    smooth(
        0,
        1,
        onlineProgress
    );

finalLabel.material.opacity =
    smooth(
        0,
        1,
        Math.max(
            0,
            onlineProgress - 0.35
        )
    );
    /* =====================================
       CHIP
    ===================================== */

    const chipProgress =
        progress(
            scrollValue,
            0,
            0.11
        );

    chipScene.scale.setScalar(
        lerp(
            1.12,
            0.52,
            chipProgress
        )
    );

    chipScene.position.z =
        lerp(
            0,
            -2.5,
            chipProgress
        );

    chipScene.rotation.y =
        time * 0.07 +
        scrollValue * 0.5;

    cpu.scale.setScalar(
        1 +
        Math.sin(time * 3) *
        0.06
    );

    cpuLight.intensity =
        6 +
        Math.sin(time * 3) *
        1.5;


    /* Data */

    dataParticles.forEach(
        (item) => {

            const t =
                (
                    time *
                    item.speed +
                    item.offset
                ) % 1;

            item.object.position.x =
                -2.8 +
                t * 5.6;
        }
    );


    /* =====================================
       UBUNTU
    ===================================== */

    const ubuntuProgress =
        progress(
            scrollValue,
            0.23,
            0.38
        );

    ubuntuScene.scale.setScalar(
        lerp(
            0.02,
            1,
            ubuntuProgress
        )
    );

    ubuntuScene.position.z =
        lerp(
            5,
            0,
            ubuntuProgress
        );

    ubuntuScene.rotation.y =
        lerp(
            -0.3,
            0,
            ubuntuProgress
        );

    ubuntuScene.position.x =
        lerp(
            -2,
            0,
            ubuntuProgress
        );

    ubuntuScene.position.y =
        Math.sin(time * 0.5) *
        0.04;


    /* =====================================
       KERNEL
    ===================================== */

    const kernelProgress =
        progress(
            scrollValue,
            0.36,
            0.50
        );

    kernelScene.scale.setScalar(
        lerp(
            0.02,
            1,
            kernelProgress
        )
    );

    kernelScene.position.z =
        lerp(
            4,
            0,
            kernelProgress
        );

    kernel.rotation.x +=
        0.002;

    kernel.rotation.y +=
        0.004;

    kernelCore.rotation.y +=
        0.005;

    kernelWire.rotation.y -=
        0.002;

    modules.forEach(
        (module, index) => {

            module.position.z =
                Math.sin(
                    time * 1.3 +
                    index
                ) * 0.1;
        }
    );


    /* =====================================
       MEMORY
    ===================================== */

    const memoryProgress =
        progress(
            scrollValue,
            0.48,
            0.62
        );

    memoryScene.scale.setScalar(
        lerp(
            0.02,
            1,
            memoryProgress
        )
    );

    memoryCells.forEach(
        (cell, index) => {

            const spread =
                lerp(
                    0.2,
                    1.7,
                    memoryProgress
                );

            cell.object.position.x =
                cell.x *
                0.4 *
                spread;

            cell.object.position.z =
                cell.z *
                0.4 *
                spread;

            cell.object.position.y =
                Math.sin(
                    time * 1.6 +
                    index
                ) *
                0.04;
        }
    );


    /* =====================================
       SCHEDULER
    ===================================== */

    const schedulerProgress =
        progress(
            scrollValue,
            0.60,
            0.73
        );

    schedulerScene.scale.setScalar(
        lerp(
            0.02,
            1,
            schedulerProgress
        )
    );

    schedulerScene.rotation.y =
        time * 0.1;


    const timer =
        (
            time * 0.55
        ) % 2;

    const target =
        timer < 1
            ? schedulerNodes[0]
            : schedulerNodes[1];

    timerPulse.position.lerp(
        target.position,
        0.08
    );


    /* =====================================
       SECURITY
    ===================================== */

    const securityProgress =
        progress(
            scrollValue,
            0.71,
            0.83
        );

    securityScene.scale.setScalar(
        lerp(
            0.02,
            1,
            securityProgress
        )
    );

    ring0.rotation.z +=
        0.0025;

    ring3.rotation.z -=
        0.0035;

    syscall.position.x =
        Math.sin(time * 1.5) *
        2;

    syscall.position.y =
        Math.cos(time * 1.5) *
        0.8;


    /* =====================================
       VFS
    ===================================== */

    const vfsProgress =
        progress(
            scrollValue,
            0.81,
            0.93
        );

    vfsScene.scale.setScalar(
        lerp(
            0.02,
            1,
            vfsProgress
        )
    );

    vfsNodes.forEach(
        (node, index) => {

            node.rotation.x =
                time * 0.15;

            node.rotation.y =
                time * 0.2;

            node.position.z =
                Math.sin(
                    time +
                    index
                ) * 0.08;
        }
    );


    /* =====================================
       FINAL CHIP
    ===================================== */

    const finalProgress =
        progress(
            scrollValue,
            0.91,
            1
        );

    finalScene.scale.setScalar(
        lerp(
            0.02,
            1,
            finalProgress
        )
    );

    finalScene.position.z =
        lerp(
            3,
            0,
            finalProgress
        );

    finalScene.rotation.y =
        time * 0.1;


    /* =====================================
       CAMERA
    ===================================== */

  const isMobile =
    window.innerWidth <= 768;

const cameraX =
    Math.sin(
        scrollValue *
        Math.PI *
        2
    ) *
    (isMobile ? 0.8 : 1.8);

const targetX =
    cameraX +
    mouseX *
    (isMobile ? 0.12 : 0.35);

const targetY =
    (isMobile ? 2.5 : 2.2) -
    scrollValue *
    (isMobile ? 1.0 : 1.6) -
    mouseY *
    (isMobile ? 0.08 : 0.16);

const targetZ =
    lerp(
        isMobile ? 15 : 12,
        isMobile ? 7.5 : 5.6,
        scrollValue
    );


    /* =====================================
       RENDER
    ===================================== */

    renderer.render(
        scene,
        camera
    );
}

animate();


/* =========================================
   RESIZE
========================================= */

window.addEventListener(
    "resize",
    () => {

        camera.aspect =
            window.innerWidth /
            window.innerHeight;

        camera.updateProjectionMatrix();

        renderer.setSize(
            window.innerWidth,
            window.innerHeight
        );
    }
);