.pragma library

function looksLikeUrl(text) {
    if(!text)
        return false;

    text = text.trim();

    return text.startsWith("http://") ||
           text.startsWith("https://") ||
           text.startsWith("ftp://");
}
