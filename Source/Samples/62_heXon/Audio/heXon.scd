s.boot();
s.quit();
//Raw Shot
(
{
	var ampEnv = Env(
		levels: [0,0.5,0.1,0.2,0,0.5,0],
		times:[0.027,0.01,0.005,0.01,0.005,0.1],
		curve: 'sin');
	var freqEnv = Env(
		levels: [[10,10],[650,623],[10,20],[230,200]],
		times:[0.07],
		curve:1);
	Pulse.ar(EnvGen.kr(freqEnv),Pulse.kr(100, SinOsc.kr(300,0,0.8, 0.2),0.2,0.5),EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Seeker
(
{
	var ampEnv = Env(
		levels: [0,0.666,0],
		times:[0.3,0.1],
		curve: -5);
	var freqEnv = Env(
		levels: [[100,101],[60,90], [151,150]],
		times:[0.2],
		curve:1);
	Saw.ar(EnvGen.kr(freqEnv),EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Shoot
(
{
	var ampEnv = Env(
		levels: [0,0.5,0],
		times:[0.015,0.07],
		curve: 1);
	var freqEnv = Env(
		levels: [900,400],
		times:[0.1],
		curve:1);
	Saw.ar(EnvGen.kr(freqEnv),EnvGen.kr(ampEnv, doneAction: 1),[0,0]);
}.play();
)

//Pickup
(
{
	var ampEnv = Env(
		levels: [0, 0.9, 0, 0.8, 0, 0.5, 0, 0.8, 0, 0.6, 0],
		times:[0.0023,0.082],
		curve: 1);
	var freqEnv = Env(
		levels: [120,230, 110, 215],
		times:[0.1],
		curve:1);
	SinOsc.ar(EnvGen.kr(freqEnv),0,EnvGen.kr(ampEnv, doneAction: 1),[0,0]);
}.play();
)

//Hit
(
{
	var ampEnv = Env(
		levels: [0,0.5,0],
		times:[0.01,0.06],
		curve: 1);
	var freqEnv = Env(
		levels: [100,110],
		times:[0.07],
		curve:1);
	Pulse.ar(EnvGen.kr(freqEnv),Pulse.kr([100,101], SinOsc.kr([100,102],0,0.25, 0.5),0.25,0.5),EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Shielded Hit
(
{
	var ampEnv = Env(
		levels: [0,5, 0.8,0],
		times:[0.023, 0.01, 0.16],
		curve: 1);
	var freqEnv = Env(
		levels: [[400,410],[411, 408], 10],
		times:[0.5],
		curve:1);
	Pulse.ar(EnvGen.kr(freqEnv),SinOsc.kr([100,101], Pulse.kr([100,102],0,0.25, 0.5),0.25,0.5),EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Explosion
(
{
var ampenv = Env(
		levels:[0,2,0],
		times:[0.08,0.6],
		curve:-1
	);

var freqenv = Env(
		[2,0,4,0],
		[0.15,0.5],
		curve:-8
	);

	SinOsc.ar(
		Pulse.kr(100,0.1,50,100)*EnvGen.kr(freqenv),
		Saw.kr(80*EnvGen.kr(freqenv),WhiteNoise.kr(8),0),
		SinOsc.kr(Pulse.kr(20,0.5,2,80),[0,3.14],0.01,0.01)*WhiteNoise.kr(3,1)
	)*EnvGen.kr(ampenv, doneAction:2);
}.play();
)

//Seeker Hit
(
play{
	var time = 0.23;
	var freq = (55).midicps;
	var a = VarSaw.ar(freq/2, width: XLine.ar(0.5,1,time)).range(0,XLine.ar(0.666,1/666,time));
	var tone = SinOsc.ar(freq).fold(-0.23*a,a) * EnvGen.ar(Env([1,0],[time]));
	Out.ar(0, tone.dup);
};
)

//Baphomech
(
play{SinOsc.ar(Pulse.kr(55, 0.23, SinOsc.ar(235,[0.0, 3.14],2,23)), Saw.ar(100, SinOsc.kr(10, 0, 0.023, 0.05)**0.666, 0.01), 0.42);}
)

//SplatterPillar
(
play{
    p=LFPulse;
    n=LFNoise1;
    Limiter.ar(
		GVerb.ar(
            {|i|BPF.ar(PinkNoise.ar(Decay.ar(Impulse.ar(0.23,0.95+n.kr(0.1,0.023)),rrand(0.2,0.4),2)).tanh,300*i+666,0.4)}.dup(6).sum*1.2,
            23, 0.23, 0.666, 0.5, 30, 1, 0.2, 0.3
        ) + Splay.ar(
            {|i|Ringz.ar(Pulse.ar(Impulse.ar(0.23,n.kr(1,0.08)),4),13*i+105+n.kr(50,23),0.13,n.kr(5).range(1,8)).tanh}.dup(23),0.05
        ).madd(0.1)
    )
}
)
