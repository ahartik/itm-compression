data <- read.csv('../h.csv', head=F)
vs <- c(195.452,362.49,134.572,105.016,159.407,72.0598,68.4516,88.515,44.6755,47.4234,50.098,37.6865,37.1555,41.2804,35.197)

draw <- function(h, m) {
	xs <- -m:m
	plot(xs, data[(2000-m):(2000+m),h]/sum(data[,h]), col=rgb(1,0,0,0.5), xlab='Value', ylab='Frequency', pch=3)
	lines(xs, dnorm(xs, 0, vs[h]))
}
makepdf <- function(h, m) {
	fname <- paste('histo',h,'.pdf',sep='')
	pdf(fname, width=5, height=5)
	draw(h,m)
	dev.off()
}
