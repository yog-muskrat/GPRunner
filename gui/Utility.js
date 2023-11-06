function calcColumnWidth(header, column) {
  let stretched = 0

  if(column == this.columns - 1) {
    let total = 0;
    for(let i = 0; i < column; i++) total += this.columnWidth(i);
    stretched = this.width - total;
  }

  let ew = this.explicitColumnWidth(column);
  if (ew > 0) return ew;

  let min = 15
  let iw = this.implicitColumnWidth(column);
  let hw = header.implicitColumnWidth(column);
  return Math.max(min, stretched, ew, iw, hw);
}
