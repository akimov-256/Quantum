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