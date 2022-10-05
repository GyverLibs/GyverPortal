function GP_hint(id, txt) {
  document.getElementById(id).title = txt;
}

function GP_send(req) {
  var xhttp = new XMLHttpRequest();
  xhttp.open('POST', req, true);
  xhttp.send();
}

function GP_clickUD(arg, dir) {
  if (arg.name) GP_send('/GP_click?' + arg.name + '=1&_dir=' + dir);
}

function GP_click(arg, r = 0) {
  if (!arg.name) return;
  var v;
  if (arg.type == 'number') {
    if (arg.hasAttribute('min') && Number(arg.value) <= Number(arg.min)) arg.value = arg.min;
    if (arg.hasAttribute('max') && Number(arg.value) >= Number(arg.max)) arg.value = arg.max;
  }
  if (arg.type == 'checkbox') v = arg.checked ? '1' : '0';
  else v = arg.value;
  if (v.charAt(0) == '#') v = v.substring(1);
  GP_send('/GP_click?' + arg.name + '=' + v);
  if (r != 0) setTimeout(function() {
    location.reload();
  }, r);
}

function GP_clickid(btn, tar) {
  GP_send('/GP_click?' + btn + '=' + document.getElementById(tar).value);
}

function GP_change(arg) {
  arg.style.backgroundSize = (arg.value - arg.min) * 100 / (arg.max - arg.min) + '% 100%';
  document.getElementById(arg.id + '_val').value = arg.value
}

function GP_wheel(arg) {
  var e = window.event;
  arg.value -= Math.sign(e.deltaY || e.detail || e.wheelDelta) * Number(arg.step);
}

function saveFile(id) {
  document.getElementById(id).click();
}

function GP_subm(id) {
  document.getElementById(id).submit();
  event.preventDefault();
}

function openTab(tab, btn, blk) {
  var x = document.getElementsByClassName(blk);
  for (var i = 0; i < x.length; i++) x[i].style.display = 'none';
  document.getElementById(tab).style.display = 'block';
  x = document.getElementsByClassName(btn.className);
  for (var i = 0; i < x.length; i++) x[i].style.background = '';
  btn.style.background = '#2a2d35';
}

function GP_spin(id, stp, dec) {
  var num = document.getElementById(id);
  num.value = (Number(num.value) + stp).toFixed(dec);
  var e = new Event('change');
  num.dispatchEvent(e);
}

function GP_update(ids) {
  var xhttp = new XMLHttpRequest();
  xhttp.open('GET', '/GP_update?' + ids + '=', true);
  xhttp.send();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var resp = this.responseText.split(',');
      ids = ids.split(',');
      if (ids.length != resp.length) return;
      for (let i = 0; i < ids.length; i++) {
        var item = document.getElementById(ids[i]);
        if (!item || !resp[i]) continue;
        if (item.type == 'hidden' && item.value == '_reload') {
          if (resp[i] == '1') location.reload();
        } else if (item.type == 'checkbox' || item.type == 'radio') item.checked = Number(resp[i]);
        else if (item.type == 'select-one') document.querySelector('#' + ids[i]).value = resp[i];
        else if (item.type == undefined) item.innerHTML = resp[i];
        else item.value = resp[i];
        if (item.type == 'range') GP_change(item);
      }
    }
  };
}