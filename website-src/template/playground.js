var query = (function () {
    var query = location.search.substr(1);
    var result = {};
    query.split('&').forEach(function (part) {
        var item = part.split('=');
        result[item[0]] = decodeURIComponent(item[1]);
    });
    return result;
})();

if (query.id) {
    document.querySelector('iframe').src = "https://jsplayground.taobao.org/vueplayground/" + query.id
}else{
    document.querySelector('iframe').src = "https://jsplayground.taobao.org/vueplayground/87ea9a43-cab1-4d3f-aa16-a0fe5e4c6539"
}