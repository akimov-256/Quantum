const activationSwitch =
    document.getElementById("activationSwitch");

const statusHighlight =
    document.getElementById("statusHighlight");


function updateStatus(isActive) {
    statusHighlight.textContent =
        isActive ? "Turn off" : "Turn on";

    statusHighlight.classList.toggle(
        "off",
        !isActive
    );
}


async function initialize() {
    const { isActive = false } = await browser.storage.local.get("isActive");

    const switchOuter = document.querySelector('.switch-outer');

    if (switchOuter) {
        switchOuter.classList.add('no-transition');
    }

    activationSwitch.checked = isActive;
    updateStatus(isActive);

    if (switchOuter) {
        void switchOuter.offsetHeight;

        requestAnimationFrame(() => {
            switchOuter.classList.remove('no-transition');
        });
    }
}


activationSwitch.addEventListener("change", async () => {
    const isActive = activationSwitch.checked;

    await browser.storage.local.set({
        isActive
    });

    updateStatus(isActive);
});


initialize();