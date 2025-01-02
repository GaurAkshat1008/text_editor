-- Function to handle created_at and updated_at timestamps on insert
CREATE OR REPLACE FUNCTION document_set_timestamps()
RETURNS TRIGGER AS $$
BEGIN
    NEW.created_at = CURRENT_TIMESTAMP;
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


-- Create triggers to automatically call these functions
CREATE OR REPLACE TRIGGER document_timestamps_trigger
    BEFORE INSERT ON documents
    FOR EACH ROW
    EXECUTE FUNCTION document_set_timestamps();


-- Optional: Function to track document version changes
CREATE OR REPLACE FUNCTION document_version_increment()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.content <> OLD.content THEN
        NEW.version = OLD.version + 1;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Create trigger for version tracking
CREATE OR REPLACE TRIGGER document_version_trigger
    BEFORE UPDATE ON documents
    FOR EACH ROW
    EXECUTE FUNCTION document_version_increment();