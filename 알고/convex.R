#! /usr/bin/env Rscript
png("convex.png", width=700, height=700)
plot(1:100, 1:100, type="n")

#points
points(58,80)
points(35,22)
points(93,84)
points(84,17)
points(95,20)
points(51,71)
points(93,90)
points(75,21)
points(41,75)
points(49,35)

#line segments
segments(58,80,93,90)
segments(58,80,41,75)
segments(35,22,84,17)
segments(35,22,41,75)
segments(84,17,95,20)
segments(95,20,93,90)
dev.off()
