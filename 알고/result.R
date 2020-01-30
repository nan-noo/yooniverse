#! /usr/bin/env Rscript
png("convex.png", width=700, height=700)
plot(1:100.000, 1:100.000, type="n")

#points
points(22.320,41.108)
points(88.559,27.157)

#line segments
segments(22.320,41.108,88.559,27.157)
segments(88.559,27.157,22.320,41.108)
dev.off()
