const hostName = "com.mouloud_guenane.quantum";
let port = null;
let pendingResolvers = [];

function getPort() {
    if (port) return port;

    port = browser.runtime.connectNative(hostName);

    port.onMessage.addListener((response) => {
        console.log("Native host response:", response);
        // Resolve whichever send is waiting on a reply
        const resolve = pendingResolvers.shift();
        if (resolve) resolve(response);
    });

    port.onDisconnect.addListener(() => {
        console.log("Native host disconnected:", browser.runtime.lastError?.message);
        port = null;
        // Reject anything still waiting so we don't hang forever
        pendingResolvers.forEach(r => r({ status: "error", message: "disconnected" }));
        pendingResolvers = [];
    });

    return port;
}

function sendAndWaitForAck(message, timeoutMs = 5000) {
    return new Promise((resolve) => {
        pendingResolvers.push(resolve);
        getPort().postMessage(message);

        setTimeout(() => resolve({ status: "timeout" }), timeoutMs);
    });
}

browser.downloads.onDeterminingFilename.addListener((downloadItem, suggest) => {
    handleInterception(downloadItem);

    suggest({
        filename: downloadItem.filename,
        conflictAction: "uniquify"
    });
});

browser.downloads.onCreated.addListener(async (downloadItem) => {
    const { isActive = false } = await browser.storage.local.get("isActive");
    if (!isActive) return;

    browser.downloads.cancel(downloadItem.id);
    await handleInterception(downloadItem);   // <-- await keeps the worker alive until this resolves
});

async function handleInterception(downloadItem) {
    const { isActive = false } = await browser.storage.local.get("isActive");
    if (!isActive) return;

    let cookieHeader = "";
    try {
        const cookies = await browser.cookies.getAll({ url: downloadItem.url });
        cookieHeader = cookies.map(c => `${c.name}=${c.value}`).join("; ");
    } catch (e) {
        console.warn("Could not read cookies for", downloadItem.url, e);
    }

    const ack = await sendAndWaitForAck({
        url: downloadItem.url,
        referrer: downloadItem.referrer || "",
        cookies: cookieHeader
    });

    console.log("Delivery result:", ack);
}