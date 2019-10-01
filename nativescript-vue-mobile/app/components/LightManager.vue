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

      <GridLayout ~mainContent columns="*" rows="400, auto,auto">
        <Label row="1" class="message" :text="msg" />
        <Button row="2" text="Toggle LED" @tap="onConnect" />
      </GridLayout>
    </RadSideDrawer>
  </Page>
</template>

<script >
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
    return {};
  },
  methods: {
    onConnect: function() {
      if (!particle.isAuthenticated()) {
        particle.loginWithToken("2b5810b22ce75c52e9be18bf88b8687b90f8a183");
      }

      this.$store.commit("connect");

      particle.listDevices().then(devices => {
        if (devices.length > 0) {
          const dockDevice = devices.find(device => {
            return device.id === "e00fce68998df7b0c4c53388";
          });

          if (dockDevice) {
            this.$store.commit("setDevice", dockDevice);

            console.log("Device Found: ", this.$store.state.device);
          } else {
            console.log("Device not found!");
          }
        }
      });
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
</style>
