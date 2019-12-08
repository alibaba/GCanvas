// Scaled rectangle
ctx.scale(9, 3);
ctx.fillStyle = 'red';
ctx.fillRect(10 *ratio, 10 *ratio, 8 *ratio, 20 *ratio);

// Reset current transformation matrix to the identity matrix
ctx.setTransform(1, 0, 0, 1, 0, 0);

// Non-scaled rectangle
ctx.fillStyle = 'gray';
ctx.fillRect(10 *ratio, 10 *ratio, 8 *ratio, 20 *ratio);