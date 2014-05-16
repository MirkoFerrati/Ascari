clear;
[names, distance, time, source, target, used, saved] = textread('test.txt', '%s %f %f %f %f %f %f', 'headerlines',1);
[lognumber,future, real] = textread('summary.txt', '%s %f %f', 'headerlines',1);