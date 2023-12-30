function calcColumnWidth(header, column) {
  let stretched = 0

  if(column == this.columns - 1) {
    let total = 0;
    for(let i = 0; i < column; i++) total += this.columnWidth(i);
    if(this.width > total) stretched = this.width - total;
  }

  let iw = this.implicitColumnWidth(column);
  let hw = header.implicitColumnWidth(column);
  let ew = this.explicitColumnWidth(column);
  if (ew > iw && ew > hw) return ew;

  let min = 15
  return Math.max(min, stretched, ew, iw, hw);
}
