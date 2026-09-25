export function initTerminal() {
    const terminal =
        document.querySelector(".terminal-body");

    if (!terminal) return;

    const lines = [
        {
            text: "ValaOS 2.0 Booting...",
            type: "title"
        },
        {
            text: "",
            type: "normal"
        },
        {
            text: "[ OK ]  GRUB loader",
            type: "success"
        },
        {
            text: "[ OK ]  Protected mode",
            type: "success"
        },
        {
            text: "[ OK ]  Memory management",
            type: "success"
        },
        {
            text: "[ OK ]  Paging and virtual memory",
            type: "success"
        },
        {
            text: "[ OK ]  Page fault handler",
            type: "success"
        },
        {
            text: "[ OK ]  Hardware timer and interrupts",
            type: "success"
        },
        {
            text: "[ OK ]  Task scheduler",
            type: "success"
        },
        {
            text: "[ OK ]  Ring 3 user-mode",
            type: "success"
        },
        {
            text: "[ OK ]  System calls",
            type: "success"
        },
        {
            text: "[ OK ]  Persistent VFS",
            type: "success"
        },
        {
            text: "[ OK ]  User programs",
            type: "success"
        },
        {
            text: "",
            type: "normal"
        },
        {
            text: "System Status: OK",
            type: "status"
        },
        {
            text: "ValaOS 2.0 Ready.",
            type: "ready"
        },
        {
            text: "",
            type: "normal"
        },
        {
            text: "ValaOS> programs",
            type: "command"
        },
        {
            text: "hello",
            type: "normal"
        },
        {
            text: "test",
            type: "normal"
        },
        {
            text: "",
            type: "normal"
        },
        {
            text: "ValaOS> run hello",
            type: "command"
        },
        {
            text: "Starting user program: hello",
            type: "normal"
        },
        {
            text: "[ USER ] sys_write: Hello from user program!",
            type: "success"
        },
        {
            text: "[ USER ] sys_exit called",
            type: "success"
        },
        {
            text: "[ OK ]  User task terminated",
            type: "success"
        }
    ];

    terminal.innerHTML = "";

    let started = false;

    function typeLine(index) {
        if (index >= lines.length) {
            addCursor();
            return;
        }

        const data = lines[index];

        const line =
            document.createElement("p");

        if (data.type === "success") {
            line.classList.add("success");
        }

        if (data.type === "title") {
            line.style.color = "#ffffff";
        }

        if (data.type === "status") {
            line.style.color = "#ff3344";
        }

        if (data.type === "ready") {
            line.style.color = "#76c893";
        }

        if (data.type === "command") {
            const commandParts =
                data.text.split(" ");

            const prompt =
                document.createElement("b");

            prompt.textContent =
                commandParts.shift();

            line.appendChild(prompt);

            line.appendChild(
                document.createTextNode(
                    " " +
                    commandParts.join(" ")
                )
            );
        } else {
            line.textContent = data.text;
        }

        terminal.appendChild(line);

        setTimeout(() => {
            typeLine(index + 1);
        }, data.text === "" ? 120 : 95);
    }

    function addCursor() {
        const cursor =
            document.createElement("span");

        cursor.className = "cursor";

        terminal.appendChild(cursor);
    }

    const observer =
        new IntersectionObserver(
            (entries, observerInstance) => {
                if (
                    entries[0].isIntersecting &&
                    !started
                ) {
                    started = true;

                    typeLine(0);

                    observerInstance.disconnect();
                }
            },
            {
                threshold: 0.25
            }
        );

    observer.observe(terminal);
}