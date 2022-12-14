function GP_canvas(str) {
  var cmd = '';
  var arr = ['fillStyle', 'strokeStyle', 'shadowColor', 'shadowBlur', 'shadowOffsetX', 'shadowOffsetY', 'lineCap', 'lineJoin', 'lineWidth', 'miterLimit', 'font', 'textAlign', 'textBaseline', 'globalAlpha', 'globalCompositeOperation', 'rect', 'fillRect', 'strokeRect', 'clearRect', 'fill', 'stroke', 'beginPath', 'moveTo', 'closePath', 'lineTo', 'clip', 'quadraticCurveTo', 'bezierCurveTo', 'arc', 'arcTo', 'scale', 'rotate', 'translate', 'fillText', 'strokeText', 'drawImage', 'save', 'restore']
  str.split(';').forEach((st) => {
    if (st.includes('::')) {
      st = st.split('::');
      var cnum = Number(st[0]);
      cmd += 'cx.' + arr[cnum] + ((cnum >= 15) ? '(' : '=') + st[1] + ((cnum >= 15) ? ');' : ';');
    } else cmd += st + ';';
  });
  return cmd;
}