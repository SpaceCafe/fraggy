# Load used libraries
library(jsonlite)
library(dplyr)
library(purrr)
library(ggplot2)

# Used filesystem
filesystem <- 'ext4'
# filesystem <- 'zfs'

# Measurement states
states <- c('initial', 'fragmented', 'defragmented', 'rsynced')
# states <- c('initial', 'fragmented', 'zfs-sent', 'rsynced')

# Drives
drives <- c('sg500', 'sg1000', 'wdgold', 'sg500-raid')
# drives <- c('sg500', 'sg1000', 'wdgold', 'sg500-raid')

# Transfer sizes
transfer_sizes <- c('10m', '100m', '1g')

for(transfer_size in transfer_sizes)
{
    statistics <- NULL
    statistics_wilcox <- NULL

    for(drive in drives)
    {
        data <- NULL
        statistic <- NULL

        for(state in states)
        {
            # Fetch measurement data from IOR as JSON file
            data_json <- fromJSON(paste('src/', filesystem, '_', drive, '_', transfer_size, '_' , state, '.json', sep = ''))

            # Filter measurement data and merge list of data frames
            data_filtered <- bind_rows(map(data_json$tests$Results[[1]], ~filter(.x, access %in% c('read', 'write'))), .id = "iteration")
            data_filtered$state = state
            data <- rbind(data, data_filtered)
        }

        data$drive = drive

        # Create box-and-whisker plot
        data %>%
            ggplot(aes(x = factor(state, levels = states), y = bwMiB, fill = factor(access))) +
                labs(x = 'State') +
                labs(y = 'Transfer Rate [MiB/s]') +
                labs(fill = 'Access') +
                scale_fill_manual(values=c('#90EE90', '#FFCC88')) +
                geom_boxplot(position = position_dodge2(padding = 0.5)) +
                geom_point(alpha = 0.3, position = position_jitterdodge()) -> image

        # Save image
        ggsave(file=paste('Plots/Performance/', filesystem, '_', drive, '_', transfer_size, '.svg', sep = ''), plot = image, width = 8, height = 5)

        # Statistics
        # Mean, SD, Rang, Shapiro-Wilk-Test, and Wilcox Tests
        statistic_mean    <- aggregate(list(mean = data$bwMiB), list(drive = data$drive, access = data$access, state = data$state), function(x) round(mean(x), digits = 2))
        statistic_sd      <- aggregate(list(sd = data$bwMiB), list(drive = data$drive, access = data$access, state = data$state), function(x) round(sd(x), digits = 2))
        statistic_range   <- aggregate(list(range = data$bwMiB), list(drive = data$drive, access = data$access, state = data$state), function(x) round(diff(range(x)), digits = 2))
        statistic_shapiro <- aggregate(list(shapiro = data$bwMiB), list(drive = data$drive, access = data$access, state = data$state), function(x) round(shapiro.test(x)$p.value, digits = 8))

        statistic  <- cbind(statistic_mean)
        statistic  <- merge(statistic, statistic_sd, by = c('drive', 'access', 'state'))
        statistic  <- merge(statistic, statistic_range, by = c('drive', 'access', 'state'))
        statistic  <- merge(statistic, statistic_shapiro, by = c('drive', 'access', 'state'))
        statistics <- rbind(statistics, statistic)

        # Wilcox Tests
        for(io in c('read', 'write'))
        {
            for(i in 1:(length(states) - 1))
            {
                statistic_wilcox     <- round(wilcox.test(filter(data, access == io, state == states[i])$bwMiB,
                                                          filter(data, access == io, state == states[i + 1])$bwMiB,
                                              paired = TRUE)$p.value, digits = 8)
                statistic_difference <- round((((mean(filter(data, access == io, state == states[i + 1])$bwMiB) /
                                                 mean(filter(data, access == io, state == states[i])$bwMiB)) - 1) * 100), digits = 0)
                statistics_wilcox    <- rbind(statistics_wilcox, c(drive, io, states[i], states[i + 1], statistic_wilcox, statistic_difference))
            }
        }
    }

    # Dump statistics to CSV
    colnames(statistics) <- c('Drive', 'IO', 'State', 'Mean', 'Standard Deviation', 'Range', 'Shapiro-Wilk Test')
    colnames(statistics_wilcox) <- c('Drive', 'IO', 'State 1', 'State 2', 'Wilcoxon Signed-Rank Test', 'Difference (pct)')
    write.csv(statistics, paste('CSV/Statistics/', filesystem, '_', transfer_size, '_basics.csv', sep = ''), row.names = FALSE, quote = FALSE)
    write.csv(statistics_wilcox, paste('CSV/Statistics/', filesystem, '_', transfer_size, '_wilcox.csv', sep = ''), row.names = FALSE, quote = FALSE)
}
