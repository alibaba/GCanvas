// Create a radial gradient
// The inner circle is at x=110, y=90, with radius=30
// The outer circle is at x=100, y=100, with radius=70
var gradient = ctx.createRadialGradient(110 * ratio,90 * ratio,30 * ratio, 100 * ratio,100 * ratio,70 * ratio);

// Add three color stops
gradient.addColorStop(0, 'pink');
gradient.addColorStop(.9, 'white');
gradient.addColorStop(1, 'green');

// Set the fill style and draw a rectangle
ctx.fillStyle = gradient;
ctx.fillRect(20 * ratio, 20 * ratio, 160 * ratio, 160 * ratio);