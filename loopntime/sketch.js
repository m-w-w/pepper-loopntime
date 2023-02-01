/***** sketch.js *****/

// Visualize the loop output in real time. Active loop has a red border.

let maxAmp = [];  
let LD = [];

function setup() {
    
    frameRate(30); // Match up with value in loop control class
    
    createCanvas(windowWidth, windowHeight);
    maxWidth = Bela.data.buffers[0][0];
    windowWidthPadded = round(windowWidth * 0.95);
    absAmplitudeArray = new Array(windowWidthPadded);
    
    let numLoops = Bela.data.buffers[0].length / 5;
    for (let i=0; i<numLoops; i++) {
        let loopNum = i;
        LD.push(new loopDisplay(numLoops, loopNum, maxWidth, windowWidth, windowHeight));
        maxAmp.push(0.001);
    }
    
}

function draw() {
    
    // Draw background
    background(color('#85dfed'));

    // Draw loop display    
    for (let i=0; i<LD.length; i++) {
        let loopLayer1Length = Bela.data.buffers[0][(i*5 + 1)];
        let readPointer = Bela.data.buffers[0][(i*5 + 2)];
        let absAmplitude = Bela.data.buffers[0][(i*5 + 3)] / 1000;
        let activeLoop = Bela.data.buffers[0][(i*5 + 4)];
        if (absAmplitude > maxAmp[i]) {maxAmp[i] = absAmplitude;}
        
        LD[i].update(activeLoop, loopLayer1Length, readPointer, absAmplitude, maxAmp[i]);
        LD[i].display();
    }

}

class loopDisplay {
    constructor(numLoops, loopNum, maxWidth, windowWidth, windowHeight){
        this.activeLoop = 0;
        this.numLoops = numLoops;
        this.loopNum = loopNum;
        this.maxWidth = maxWidth;
        this.windowWidthPadded = round(windowWidth * .95);
        this.absAmplitudeArray = new Array(this.windowWidthPadded);
        
        this.centerX = windowWidth/2;
        this.centerY = windowHeight/(numLoops+1)*(loopNum+1); 
        
        this.rectWidth = 0;
        this.rectX1 = this.centerX - this.rectWidth/2;
        this.rectHeight = round(windowHeight/(numLoops + 2));

        this.loopLayer1Length = 0;
        this.readPointer = 0;
        this.lineProg = 0;
        this.absAmplitude = 0;
        this.maxAmp = .001;

        this.resetabsAmplitudeArray = false;
    
    }
    
    update(activeLoop, loopLayer1Length, readPointer, absAmplitude, maxAmp){
        
        this.activeLoop = activeLoop;
        this.loopLayer1Length = loopLayer1Length;
        this.absAmplitude = absAmplitude;
        this.maxAmp = maxAmp;
        this.rectWidth = 0;
        this.lineProg = 0;
        
        if(this.loopLayer1Length === 0 & this.resetabsAmplitudeArray){ 
            this.absAmplitudeArray = new Array(this.windowWidthPadded);
            this.rectWidth = 0;
            this.rectX1 = this.centerX - this.rectWidth/2;
            this.resetabsAmplitudeArray = false;
            
        }
        if(this.loopLayer1Length > 0){ 
            this.readPointer = readPointer;
            this.absAmplitudeArray[this.readPointer] = abs(this.absAmplitude) * this.rectHeight/2;
            this.rectWidth = (this.loopLayer1Length/this.maxWidth) * this.windowWidthPadded;
            this.lineProg = this.readPointer/this.loopLayer1Length*this.rectWidth; 
            this.resetabsAmplitudeArray = true;
        }        
        
    }
    
    display(){
        
        this.rectX1 = this.centerX - this.rectWidth/2;

        // Draw amplitude envelope
        stroke(color('#fa703a')); 
        fill(color('#f9bb54'));
        strokeWeight(2);
        
        beginShape(); // upper half
        for(let i=0; i<this.loopLayer1Length; i++) {
            vertex(i*(this.rectWidth/this.loopLayer1Length)+this.rectX1, this.centerY - this.absAmplitudeArray[i]/this.maxAmp*0.75);
        }
        vertex(this.rectX1+this.rectWidth, this.centerY);
        vertex(this.rectX1, this.centerY);
        endShape();
        
        beginShape(); // lower half
        for(let i=0; i<this.loopLayer1Length; i++) {
            vertex(i*(this.rectWidth/this.loopLayer1Length)+this.rectX1, this.centerY + this.absAmplitudeArray[i]/this.maxAmp*0.75);
        }
        vertex(this.rectX1+this.rectWidth, this.centerY);
        vertex(this.rectX1, this.centerY);
        endShape();
        
        // Draw rectangle
        strokeWeight(10);
        if(this.activeLoop===0){
            stroke(color('#d2e5f3'));
        } else {
            stroke(color('#fc3643'));
        }
        noFill();
        rectMode(CORNER);
        rect(this.rectX1-5, this.centerY-this.rectHeight/2, this.rectWidth+5, this.rectHeight); // expand due to stroke weight
    
        // Draw read pointer line
        strokeWeight(5);
        stroke(color('#fc3643'));
        line(this.lineProg+this.rectX1, this.centerY-this.rectHeight/2, this.lineProg+this.rectX1, this.centerY+this.rectHeight/2);

    }

}
