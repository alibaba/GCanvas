var query = (function () {
    var query = location.search.substr(1);
    var result = {};
    query.split('&').forEach(function (part) {
        var item = part.split('=');
        result[item[0]] = decodeURIComponent(item[1]);
    });
    return result;
})();

console.warn(document.querySelector('iframe'));
if (query.id) {
    document.querySelector('iframe').src = "https://jsplayground.taobao.com/vueplayground/" + id
}else{
    document.querySelector('iframe').src = "https://jsplayground.taobao.com/vueplayground/87ea9a43-cab1-4d3f-aa16-a0fe5e4c6539"
}