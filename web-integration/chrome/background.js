chrome.downloads.onCreated.addListener(async (downloadItem) => {
    const { isActive = false } =
        await chrome.storage.local.get("isActive");

    if (!isActive) {
        return;
    }

    // Cancel the download for the browser
    chrome.downloads.cancel(downloadItem.id)
    // Call the send function
    console.log("url: " + downloadItem.url)
    sendToManager(downloadItem.url)
});

async function sendToManager(url) {
    const response = await fetch(                       // Track a response variable
        "http://127.0.0.1:8421/download",                      // Set the target URL
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                url: url
            })                   // Set the download URL as the body
        }
    );
    if (response.ok) {                                  // Handle send success
        console.log("Url received successfully")
    } else {                                              // Handle send failure
        console.log("Url not received")
    }
}