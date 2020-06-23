#!/bin/bash
count=0
FILE=0

for i in {1..9}
do
	xcf2png -g BG.xcf $i > 00000000$i.png
	for j in {1..9}
	do
	xcf2png -g BG.xcf $i $j > 0000000$i$j.png
		for k in {1..9}
		do
		xcf2png -g BG.xcf $i $j $k > 000000$i$j$k.png
			for l in {1..9}
			do
			xcf2png -g BG.xcf $i $j $k $l > 00000$i$j$k$l.png
				for m in {0}
				do
				xcf2png -g BG.xcf $i $j $k $l $m > 0000$i$j$k$l$m.png
					for n in {0}
					do
					xcf2png -g BG.xcf $i $j $k $l $m $n > 000$i$j$k$l$m$n.png
						for o in {0}
						do
						xcf2png -g BG.xcf $i $j $k $l $m $n $o > 00$i$j$k$l$m$n$o.png
							for p in {0}
							do
							xcf2png -g BG.xcf $i $j $k $l $m $n $o $p > 0$i$j$k$l$m$n$o$p.png
								for q in {0}
								do
								xcf2png -g BG.xcf $i $j $k $l $m $n $o $p $q > $i$j$k$l$m$n$o$p$q.png
								done
							done
						done
					done
				done
			done
		done
	done
	
done


