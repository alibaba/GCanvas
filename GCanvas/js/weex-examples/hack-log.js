function hack(context) {
    
        var kmap = {};
        for (var k in context) {
            if (typeof context[k] === 'number' && k.toUpperCase() === k) {
                var v = context[k].toString();
                if (kmap[v]) {
                    kmap[v] += '/' + k;
                } else {
                    kmap[v] = '('+v+')'+k;
                }
            }
        }
    
        var makeArray = function (args) {
            var res = [];
            for (var i = 0; i < args.length; i++) {
                res.push(args[i]);
            }
            return res;
        }
    
        for (var k in context) {
    
            if (context[k] && typeof context[k] === 'function') {

                (function () {
                    var key = k;
                    var f = context[key];
                    context[key] = function (rk) {

                        var str = makeArray(arguments).map(function (item) {
                            if (typeof item === 'number' && kmap[item.toString()]) {
                                return kmap[item.toString()];
                            } else {
                                return item;
                            }
                        }).join(',');
                        if (str.length < 500) {
                            console.log('=== webgl call {' + key + '}' + str);
                        } else {
                            console.log('=== webgl call {' + key + '} too long args ' + str.length);
                        }
                        return f.apply(this, arguments);
                    }
                })();
            }
        }
    }
    
    export default hack;