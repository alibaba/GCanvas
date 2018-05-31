var gulp = require('gulp');
var tasksFactory = require('dalaran');

var tasks = tasksFactory.libraryTasks({
    entry: './src/index.js',
    umdName: 'GCanvas',
    eslint: false,
    dist:'../../hap-canvas/src/Canvas'
})

gulp.task('build', tasks.build);