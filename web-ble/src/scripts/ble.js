let data = null;

async function init(vm) {
  data = vm.$data;
  try {
    let progressItems = data.progressItems;

    progressItems.push('Requesting bluetooth device.');
    const device = await navigator.bluetooth.requestDevice({
      filters: [{ services: ['5c1b9a0d-b5be-4a40-8f7a-66b36d0a5176'] }]
    });

    progressItems.push('Connecting to GATT server.');
    const server = await device.gatt.connect();

    data.deviceFound = true;

    progressItems.push('Getting private service.');
    const service = await server.getPrimaryService('5c1b9a0d-b5be-4a40-8f7a-66b36d0a5176');

    progressItems.push('Getting private uptime characteristic.');
    await addCharacteristic(service, 'fdcf4a3f-3fed-4ed2-84e6-04bbb9ae04d4', handleUptimeNotifications);

    progressItems.push('Getting private signalStrength characteristic.');
    await addCharacteristic(service, 'cc97c20c-5822-4800-ade5-1f661d2133ee', handleSignalStrengthNotifications);

    progressItems.push('Getting private freeMemory characteristic.');
    await addCharacteristic(service, 'd2b26bf3-9792-42fc-9e8a-41f6107df04c', handleFreeMemoryNotifications);

    progressItems.push('Getting private temperature characteristic.');
    await addCharacteristic(service, '8692609a-27ce-4b35-bfc6-b6e3cd8fc5b8', handleTempNotifications);

    progressItems.push('Getting private humidity characteristic.');
    await addCharacteristic(service, '16f99aec-1fd3-4aa9-86f6-836028324711', handleHuNotifications);

    progressItems.push('Getting private pressure characteristic.');
    await addCharacteristic(service, '13b7bf7e-6382-4c93-a2e9-922e04d83bc4', handlePressNotifications);

    progressItems.push('Getting private airQuality characteristic.');
    await addCharacteristic(service, '5bfd8a03-2a5b-443d-b873-8ca6fc558134', handleAQNotifications);


    setTimeout(removeItems, 2000);
  } catch (error) {
    data.errorMsg = error;
    data.error = true;
  }
}

async function addCharacteristic(service, uuid, callback) {
  var characteristic = await service.getCharacteristic(uuid);
  await characteristic.startNotifications();
  characteristic.addEventListener('characteristicvaluechanged', callback);
}

function removeItems() {
  data.progressItems.shift();
  if (data.progressItems.length > 0) {
    setTimeout(removeItems, 500);
  }
}

function handleUptimeNotifications(event) {
  const value = event.target.value.getUint8(0);

  data.uptime = value;
}

function handleSignalStrengthNotifications(event) {
  const value = event.target.value.getUint8(0);

  data.signalStrength = value;
}

function handleFreeMemoryNotifications(event) {
  const value = event.target.value.getInt32(0);

  data.freeMemory = value / 1000;
}

function handleTempNotifications(event) {
  const value = event.target.value.getUint8(0);

  data.temp = value;
}

function handlePressNotifications(event) {
  const value = event.target.value.getUint8(0);

  data.pressure = value;
}

function handleHuNotifications(event) {
  const value = event.target.value.getUint8(0);

  data.humidity = value;
}

function handleAQNotifications(event) {
  var enc = new TextDecoder("utf-8");
  const value = event.target.value.buffer;

  data.airQuality = enc.decode(value);
}

export default {
  initBLE: init
}