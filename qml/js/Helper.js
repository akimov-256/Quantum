.pragma library

function looksLikeUrl(text) {
    if (!text)
        return false;

    text = text.trim();

    return text.startsWith("http://") ||
           text.startsWith("https://") ||
           text.startsWith("ftp://");
}

function formatFileSize(size) {
    if (size === 0)
        return "Unknown";

    if (size < 1024)
        return String(size) + " B";
    if (size < 1024 * 1024)
        return String((size / 1024).toFixed(2)) + " KB";
    if (size < 1024 * 1024 * 1024)
        return String((size / (1024 * 1024)).toFixed(2)) + " MB";
    return (size / (1024 * 1024 * 1024)).toFixed(2) + " GB";
}

function formatSpeed(bytesPerSecond) {
    if (bytesPerSecond <= 0)
        return "0 B/s";

    if (bytesPerSecond < 1024)
        return String(bytesPerSecond) + " B/s";
    if (bytesPerSecond < 1024 * 1024)
        return String((bytesPerSecond / 1024).toFixed(2)) + " KB/s";
    if (bytesPerSecond < 1024 * 1024 * 1024)
        return String((bytesPerSecond / (1024 * 1024)).toFixed(2)) + " MB/s";
    return (bytesPerSecond / (1024 * 1024 * 1024)).toFixed(2) + " GB/s";
}

function handleRTA(bytesPerSecond, downloadedBytes, totalBytes) {
    if (bytesPerSecond === 0)
        if (downloadedBytes === 0)
            return "Calculating";
        else
            return "Unknown";

    let remainingSeconds = ((totalBytes - downloadedBytes) / bytesPerSecond).toFixed(0);
    let remainingMinutes = 0;

    let result = "";

    while (remainingSeconds >= 60) {
        remainingSeconds -= 60;
        remainingMinutes += 1;
    }

    if (remainingMinutes > 0)
        result += String(remainingMinutes) + " min ";

    if (remainingSeconds < 0)
        remainingSeconds = 0;

    result += String(remainingSeconds) + " s";

    return result;
}

function formatFilePaths(url) {
    let path = url.toString();
    path = decodeURIComponent(path);

    if (path.startsWith("file:///"))
        return path.substring(8);
    else if (path.startsWith("file://"))
        return path.substring(7);
    return path;
}

function getNumberFromStr(str) {
    return String(str);
}

function getThreadSelectionModel(fileSize) {
    // Convert file size to Mb
    let fileMbSize = fileSize / (1024 * 1024);

    // Dynamically set the available thread range to maintain stabillity
    if (fileMbSize < 20)
        return [ "1" ];                 // Use only one thread for downloads under 20Mb.
    if (fileMbSize < 40)
        return [ "1", "2" ];            // Use a maximum of 2 threads for downloads between 20-40Mb.
    return ["1", "2", "4", "8", "16"];  // Use the default number of threads for downloads over 40Mb.
}
