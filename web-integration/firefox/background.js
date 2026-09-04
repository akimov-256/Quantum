browser.downloads.onCreated.addListener(async (downloadItem) => {
    const { isActive = false } =
        await browser.storage.local.get("isActive");

    if (!isActive) {
        return;
    }

    await browser.downloads
        .erase({ id: downloadItem.id });        // Immediately cancel the download.

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