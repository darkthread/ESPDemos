var vm = Vue.createApp({
    data() {
        return {
            LedSwitch: false,
            Text: "test"
        }
    },
    watch: {
        LedSwitch: (v) => $.post('/led/switch?v=' + (v ? "on" : "off"))
    },
    methods: {
    }

}).mount('#app');
