<template>
  <Page>
    <ActionBar>
      <GridLayout width="100%" columns="auto, *">
        <Label text="..." @tap="$refs.drawer.nativeView.showDrawer()" col="0" />
        <Label class="title" text="Particle Dock Controller" col="1" />
      </GridLayout>
    </ActionBar>

    <RadSideDrawer ref="drawer">
      <StackLayout ~drawerContent backgroundColor="#ffffff">
        <Label class="drawer-header" text="Dock Options" />

        <Button class="drawer-item" text="Env Vals" @tap="$navigateTo(envValsPage)" />
        <Button class="drawer-item" text="Light Manager" @tap="$navigateTo(lightManagerPage)" />
        <Button class="drawer-item" text="OLED Controller" @tap="$navigateTo(oledPage)" />
      </StackLayout>

      <StackLayout ~mainContent>
        <Label class="message" :text="msg" />
        <Button text="Connect to Dock" @tap="onConnect" v-if="!connected" />
        <FlexboxLayout class="pad" flexDirection="column" v-if="connected">
          <Slider minValue="0" maxValue="360" v-model="ledHue" />
          <Button text="Toggle LED" @tap="onToggle" />
        </FlexboxLayout>
        <StackLayout v-if="connected">
          <Button class="pad" text="Fetch Env Values" @tap="fetchEnvVals" />
          <FlexboxLayout v-if="temp > 0">
            <Label width="50%" class="flex-label" text="Temp: " />
            <Label width="50%" class="flex-value" :text="temp + ' C'" />
          </FlexboxLayout>
          <FlexboxLayout v-if="hu > 0">
            <Label width="50%" class="flex-label" text="Humidity: " />
            <Label width="50%" class="flex-value" :text="hu + '%'" />
          </FlexboxLayout>
          <FlexboxLayout v-if="pressure > 0">
            <Label width="50%" class="flex-label" text="Pressure: " />
            <Label width="50%" class="flex-value" :text="pressure + ' hPa'" />
          </FlexboxLayout>
          <FlexboxLayout v-if="airQuality != ''">
            <Label width="50%" class="flex-label" text="Air Quality: " />
            <Label width="50%" class="flex-value" :text="airQuality" />
          </FlexboxLayout>

          <Button class="pad" text="Set OLED Text" @tap="setOLEDText" />
          <TextField width="50%" v-model="oledText" hint="Enter text..." />
        </StackLayout>
      </StackLayout>
    </RadSideDrawer>
  </Page>
</template>

<script >
import environment from "../environment.json";

import {
  Particle,
  TNSParticleDevice,
  TNSParticleEvent
} from "nativescript-particle";

const particle = new Particle();

export default {
  computed: {
    connected() {
      return this.$store.state.connected;
    },
    msg() {
      return this.$store.state.connected ? "Connected!" : "Tap to connect";
    },
    dockDevice() {
      return this.$store.state.device;
    }
  },
  data() {
    return {
      ledHue: 40,
      temp: 0,
      hu: 0,
      pressure: 0,
      airQuality: "",
      oledText: ""
    };
  },
  methods: {
    onConnect: function() {
      if (!particle.isAuthenticated()) {
        particle.loginWithToken(environment.app.token);
      }

      this.$store.commit("connect");

      particle.listDevices().then(devices => {
        if (devices.length > 0) {
          const dockDevice = devices.find(device => {
            return device.id === "e00fce68998df7b0c4c53388";
          });

          if (dockDevice) {
            this.$store.commit("setDevice", dockDevice);
          } else {
            console.log("Device not found!");
          }
        }
      });
    },
    onToggle: function() {
      const device = this.$store.state.device;

      device
        .callFunction("toggleLed", this.$data.ledHue)
        .then(result => console.log("Res: ", result))
        .catch(error => console.log("Err: ", error));
    },
    fetchEnvVals: function() {
      const device = this.$store.state.device;

      device.getVariable("temp").then(result => (this.$data.temp = result));
      device.getVariable("humidity").then(result => (this.$data.hu = result));
      device
        .getVariable("pressure")
        .then(result => (this.$data.pressure = result));
      device
        .getVariable("airQuality")
        .then(result => (this.$data.airQuality = result));
    },
    setOLEDText: function() {
      const device = this.$store.state.device;

      device
        .callFunction("setText", this.$data.oledText)
        .then(result => console.log("Res: ", result))
        .catch(error => console.log("Err: ", error));
    }
  }
};
</script>

<style scoped>
ActionBar {
  background-color: #53ba82;
  color: #ffffff;
}

.title {
  text-align: left;
  padding-left: 16;
}

.message {
  vertical-align: center;
  text-align: center;
  font-size: 20;
  color: #333333;
}

.drawer-header {
  padding: 50 16 16 16;
  margin-bottom: 16;
  background-color: #53ba82;
  color: #ffffff;
  font-size: 24;
}

.drawer-item {
  padding: 8 16;
  color: #333333;
  font-size: 16;
}

StackLayout {
  padding-top: 200;
}

.pad {
  margin-top: 60;
}

.flex-label {
  text-align: right;
  padding-right: 5;
}

.flex-value {
  text-align: left;
}
</style>
