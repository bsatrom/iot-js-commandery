import Vue from 'vue';
import Vuex from 'vuex';

Vue.use(Vuex);

export default new Vuex.Store({
  state: {
    connected: false,
    device: {}
  },
  mutations: {
    connect: (state) => {
      state.connected = true
    },
    setDevice: (state, connectedDevice) => {
      state.device = connectedDevice
    }
  },
  actions: {

  }
});
