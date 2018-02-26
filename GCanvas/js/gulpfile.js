var gulp = require('gulp');
var tasksFactory = require('dalaran');

var tasks = tasksFactory.libraryTasks({
    entry: './src/index.js',
    umdName: 'GCanvas'
})

gulp.task('build', tasks.build);