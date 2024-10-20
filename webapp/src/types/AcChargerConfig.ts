export interface AcChargerConfig {
    enabled: boolean;
    verbose_logging: boolean;
    can_controller_frequency: number;
    auto_power_enabled: boolean;
    auto_power_batterysoc_limits_enabled: boolean;
    voltage_limit: number;
    enable_voltage_limit: number;
    lower_power_limit: number;
    upper_power_limit: number;
    emergency_charge_enabled: boolean;
    stop_batterysoc_threshold: number;
    target_power_consumption: number;
}

export interface AcChargerShellyConfig {
    enabled: boolean;
    verbose_logging: boolean;
    auto_power_batterysoc_limits_enabled: boolean;
    emergency_charge_enabled: boolean;
    stop_batterysoc_threshold: number;
    url: string;
    power_on_threshold: number;
    power_off_threshold: number;
}
