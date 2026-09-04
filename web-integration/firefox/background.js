browser.downloads.onCreated.addListener(async (downloadItem) => {
    const { isActive = false } =        // Get if the integration is active.
        await browser.storage.local.get("isActive");

    if (!isActive) {                            // Skip the handling if the integration is inactive.
        return;
    }

    await browser.downloads                     // Immediately cancel the download.
        .cancel(downloadItem.id);

    await browser.downloads                     // Erase the download from the disk.
        .erase({ id: downloadItem.id });

    sendToManager(downloadItem.url);            // Pass url to send function.
});

async function sendToManager(url) {
    try {
        const response = await fetch("http://127.0.0.1:8421/download", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ url: url })
        });

        if (response.ok) {
            console.log("Url received successfully");
        } else {
            console.log("Url not received");
        }
    } catch (error) {
        console.error("Fetch failed:", error);
    }
}