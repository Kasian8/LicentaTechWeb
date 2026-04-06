/**
 *
 * Mapare servo → element UI:
 *   s1 (pin 12) → Bedroom Window(1/2 double window)
 *   s2 (pin 13) → Bedroom Window(2/2 double window) 
 *   s3 (pin 14) → Bathroom Window 
 *   s4 (pin 27) → Garage Door 
 *   s5 (pin 33) → Main Door 
 */




const livingSlider = document.getElementById("living-intensity-slider"); //val input
const livingToggle   = document.getElementById("living-toggle");  //checkbox

let garageDoorState = 'closed';
let mainDoorState   = 'closed';
let isDoorLocked    = true;
let isDoorOpen      = false;

    //Functii pentru controlul usilor si geamurilor
function lockUnlock(){
    const lockUnlockBtn = document.getElementById('lockUnlockBtn');
    const mainStatusText= document.getElementById('main-door-status');


    if (!isDoorLocked && isDoorOpen) {
        return;
    }
    isDoorLocked = !isDoorLocked;

    if (isDoorLocked) {
        lockUnlockBtn.textContent = 'Tap To Unlock';
        mainStatusText.textContent = 'Locked';
    } else {
        lockUnlockBtn.textContent = 'Tap To Lock';
        mainStatusText.textContent = 'Unlocked';
    }
}

function toggleMainDoor() {
    
   
    const btn = document.getElementById('main-btn');
    if (mainDoorState === 'closed') {
        if (isDoorLocked) {
        return;
    }
        cmd('/s5/open');
        mainDoorState = 'open';
        btn.textContent = 'Close';
        isDoorOpen = true;
    } else {
        cmd('/s5/close');
        mainDoorState = 'closed';
        btn.textContent = 'Open';
        isDoorOpen = false;
    }
}



function toggleGarageDoor() {
    const btn = document.getElementById('garage-btn');
    if (garageDoorState === 'closed') {
        // Door is closed → send open command
        cmd('/s4/open');
        garageDoorState = 'open';
        btn.textContent = 'Close';
    } else {
        // Door is open → send close command
        cmd('/s4/close');
        garageDoorState = 'closed';
        btn.textContent = 'Open';
    }
}




//functii pentru controlul ledurilor
function toggleLight(checkbox, room) {
    const checkState= checkbox.checked ? 'on' : 'off';
    cmd(`/${room}/${checkState}`);
}

function controlBedroom(action) {
    cmd(`/bedroom/${action}`);
}




function toggleLivingIntensity(checkbox) {
    const valueLabel = document.getElementById("living-intensity-value");

    if (checkbox.checked) {
        cmd(`/living/on`);
        livingSlider.value = 100;
        valueLabel.innerText = "100%";
    }
    else {
        cmd(`/living/off`);
        livingSlider.value = 0;
        valueLabel.innerText = "0%";
    }
    
}
if (livingSlider) {
    livingSlider.addEventListener("change", function() {
       let val = livingSlider.value;
       const valueLabel = document.getElementById("living-intensity-value");
       valueLabel.innerText = val + "%";
       
        if(livingToggle) {
            livingToggle.checked = val > 0;
        }

        if (val == 0) {
            cmd(`/living/off`);
        }
        else if (val == 100) {
            cmd(`/living/on`);
        }
        else {          
            cmd(`/living/${val}`);
        }
    });
}


// Preluare Temperatură și Umiditate (la 5 secunde)
function fetchDHTData() {
    fetch('/sensor/dht')
        .then(response => {
            if (!response.ok) throw new Error('Eroare DHT');
            return response.json();
        })
        .then(data => {
            if (data.error) return;

            const tempEl = document.getElementById('temp-value');
            const humEl  = document.getElementById('hum-value');

            if (tempEl) tempEl.textContent = data.temperature.toFixed(1);
            if (humEl)  humEl.textContent  = data.humidity.toFixed(1);
        })
        .catch(error => console.error('Eroare DHT:', error));
}

// Preluare Gaz (la 2 secunde)
function fetchGasData() {
    fetch('/sensor/gas')
        .then(response => {
            if (!response.ok) throw new Error('Eroare Gaz');
            return response.json();
        })
        .then(data => {
            if (data.error) return;

            const gasEl = document.getElementById('gas-text');
            const gasCircle = document.getElementById('gass-circle');

            if (gasEl) {
                let gasValue = data.gas;

                if (gasValue < 20) {
                    gasEl.textContent = 'NORMAL';
                    gasEl.style.color = 'var(--success-600)';
                    gasCircle.style.fill = 'var(--success-600)';
                    gasCircle.style.stroke = 'var(--success-600)';
                }
                else if (gasValue >= 20 && gasValue < 50) {
                    gasEl.textContent = 'WARNING';
                    gasEl.style.color = 'var(--primary)';
                    gasCircle.style.fill = 'var(--primary)';
                    gasCircle.style.stroke = 'var(--primary)';
                }
                else if (gasValue >= 50) {
                    gasEl.textContent = 'DANGER';
                    gasEl.style.color = 'var(--error)';
                    gasCircle.style.fill = 'var(--error)';
                    gasCircle.style.stroke = 'var(--error)';
                }
            }
        })
        .catch(error => console.error('Eroare Gaz:', error));
}

function fetchMotionData() {
    fetch('/sensor/motion')
        .then(response => {
            if (!response.ok) throw new Error('Eroare Motion');
            return response.json();
        })
        .then(data => {
            const motionTitle = document.getElementById('motion-title');
            const motionTime = document.getElementById('motion-time');
            
            if (motionTitle && motionTime) {
                let secs = data.motion; 

                // Schimbă titlul doar cât timp e "Now" (primele 2 secunde)
                if (secs < 2) {
                    motionTitle.textContent = 'Motion Detected';
                    motionTitle.style.color = 'inherit';
                } else {
                    motionTitle.textContent = 'No Motion';
                    motionTitle.style.color = 'gray';
                }

                // Afișează secundele exacte pentru testare
                if (secs < 2) {
                    motionTime.textContent = "Now";
                } else {
                    motionTime.textContent = secs + " seconds ago";
                }
            }
        })
        .catch(error => {
            console.error('Eroare Motion:', error);
        });
}

// Apelăm inițial ambele funcții
fetchDHTData();
fetchGasData();
fetchMotionData();


// Setăm timpii diferiți
setInterval(fetchDHTData, 5000); // 5 secunde
setInterval(fetchGasData, 2000); // 2 secunde
setInterval(fetchMotionData, 2000); // 2 secunde


function cmd(path) {
    fetch(path)
        .then(response => console.log('Command sent:',path))
        .catch(error => console.error('Error sending command:', error));
}


