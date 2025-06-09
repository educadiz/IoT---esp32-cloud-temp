const functions = require('firebase-functions');
const express = require('express');
const cors = require('cors');   
const app = express();

app.use(cors({ origin: true }));  
app.use(express.json());

let ultimaLeitura = null;

app.post('/registrar', (req, res) => {
    const { temperatura, umidade } = req.body;
    console.log('Recebido POST com corpo:', req.body);

    if (
        temperatura === undefined || umidade === undefined ||
        isNaN(temperatura) || isNaN(umidade)
    ) {
        return res.status(400).send('Dados inválidos. Envie temperatura e umidade como números.');
    }

    ultimaLeitura = {
        temperatura: Number(temperatura),
        umidade: Number(umidade),
        timestamp: new Date().toISOString()
    };

    console.log('Nova leitura registrada:', ultimaLeitura);

    res.status(200).send('Dados registrados com sucesso');
});

app.get('/ultima', (req, res) => {
    if (!ultimaLeitura) {
        return res.status(404).send('Nenhuma leitura disponível');
    }
    res.json(ultimaLeitura);
});

exports.registrarTemperatura = functions.https.onRequest(app);
