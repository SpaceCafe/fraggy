# Load used libraries
library(dplyr)
library(ggplot2)

# Used filesystem
filesystem <- 'ext4'
# filesystem <- 'zfs'

# Measurement states
states <- c('fragmented', 'defragmented', 'rsynced')
# states <- c('fragmented', 'zfs-sent', 'rsynced')

# Drives
drives <- c('sg500', 'sg1000', 'wdgold', 'sg500-raid')
# drives <- c('sg500', 'sg1000', 'wdgold', 'sg500-raid')

statistics <- NULL
statistics_wilcox <- NULL

for(drive in drives)
{
    data <- NULL
    statistic <- NULL

    for(state in states)
    {
        # Fetch measurement data from Fraggy as CSV file
        data_csv <- read.csv(paste('src/', filesystem, '_', drive, '_', state ,'.csv', sep=''))
        data_csv$state = state
        data <- rbind(data, data_csv)
    }

    data$drive = drive

    # Create lineplot
    data %>%
        ggplot(aes(x = fragments, y = amount, color = factor(state, levels = states), fill = factor(state, levels = states))) +
            labs(x = 'Fragments per File') +
            labs(y = 'Frequency') +
            labs(color = 'State', fill = 'State') +
            theme(legend.position="bottom") +
            scale_color_manual(values=c('#000000', '#FF8B83', '#ADD8E6')) +
            scale_fill_manual(values=c('#000000', '#FF8B83', '#ADD8E6')) +
            scale_y_log10() +
            xlim(0,400) +
            geom_freqpoly(stat = "identity", position = "identity", na.rm = TRUE) -> image

    # Save image
    ggsave(file=paste('Plots/Fragments/', filesystem, '_', drive, '.svg', sep = ''), plot = image, width = 10, height = 5)

    # Statistics
    # SUM =1, SUM >5, SUM >10, and MAX
    data_sum1       <- filter(data, fragments == 1)
    data_sum5       <- filter(data, fragments > 5)
    data_sum10      <- filter(data, fragments > 10)
    statistic_sum1  <- aggregate(list(sum1 = data_sum1$amount), list(state = data_sum1$state, drive = data_sum1$drive), function(x) sum(x))
    statistic_sum5  <- aggregate(list(sum5 = data_sum5$amount), list(state = data_sum5$state, drive = data_sum5$drive), function(x) sum(x))
    statistic_sum10 <- aggregate(list(sum10 = data_sum10$amount), list(state = data_sum10$state, drive = data_sum10$drive), function(x) sum(x))
    statistic_max   <- aggregate(list(max = data$fragments), list(state = data$state, drive = data$drive), function(x) max(x))

    statistic  <- cbind(statistic_sum1)
    statistic  <- merge(statistic, statistic_sum5, by = c('drive', 'state'))
    statistic  <- merge(statistic, statistic_sum10, by = c('drive', 'state'))
    statistic  <- merge(statistic, statistic_max, by = c('drive', 'state'))
    statistics <- rbind(statistics, statistic)
}

# Dump statistics to CSV
colnames(statistics) <- c('Drive', 'State', 'SUM == 1 Frags', 'SUM >5 Frags' , 'SUM >10 Frags', 'MAX Frags')
write.csv(statistics, paste('CSV/Statistics/', filesystem, '_fragments.csv', sep = ''), row.names = FALSE, quote = FALSE)
