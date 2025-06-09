const tempEl = document.getElementById('temp');
const umidadeEl = document.getElementById('umidade');
const horaEl = document.getElementById('hora');
const errorEl = document.getElementById('error');
const weatherIconEl = document.getElementById('weather-icon');

const endpoint = 'https://us-central1-esp32cloudtemp.cloudfunctions.net/registrarTemperatura/ultima';

function isDaytime() {
  const hours = new Date().getHours();
  return hours >= 6 && hours < 18;
}

function getWeatherIcon(temperatura, umidade) {
  if (!temperatura || !umidade) {
    return 'images/error.png';
  }

  const period = isDaytime() ? 'day' : 'night';
  
  try {
    if (temperatura > 30) return `images/${period}_hot.png`;
    if (temperatura >= 25 && umidade > 70) return `images/${period}_cloudy.png`;
    if (umidade > 80) return `images/${period}_rain.png`;
    if (temperatura >= 20 && temperatura <= 25) return `images/${period}_cloudy.png`;
    return `images/${period}_cold.png`;
  } catch (error) {
    console.error('Erro ao definir ícone:', error);
    return 'images/error.png';
  }
}

async function buscarUltimaLeitura() {
  try {
    const res = await fetch(endpoint);
    if (!res.ok) throw new Error('Nenhuma leitura disponível');
    
    const data = await res.json();
    if (!data?.temperatura || !data?.umidade) {
      throw new Error('Dados inválidos recebidos');
    }

    const temp = parseFloat(data.temperatura);
    const umid = parseFloat(data.umidade);

    tempEl.textContent = temp.toFixed(1);
    umidadeEl.textContent = umid.toFixed(1);
    horaEl.textContent = new Date(data.timestamp).toLocaleString();
    weatherIconEl.src = getWeatherIcon(temp, umid);
    errorEl.style.display = 'none';
  } catch (err) {
    tempEl.textContent = '--';
    umidadeEl.textContent = '--';
    horaEl.textContent = '--';
    errorEl.textContent = err.message;
    errorEl.style.display = 'block';
    weatherIconEl.src = 'images/error.png';
  }
}

buscarUltimaLeitura();
setInterval(buscarUltimaLeitura, 5000);
