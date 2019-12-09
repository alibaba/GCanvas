// Moved square
ctx.translate(110 *ratio, 30 *ratio);
ctx.fillStyle = 'red';
ctx.fillRect(0, 0, 80 *ratio, 80 *ratio);

// Reset current transformation matrix to the identity matrix
ctx.setTransform(1, 0, 0, 1, 0, 0);

// Unmoved square
ctx.fillStyle = 'gray';
ctx.fillRect(0, 0, 80 *ratio, 80 *ratio);